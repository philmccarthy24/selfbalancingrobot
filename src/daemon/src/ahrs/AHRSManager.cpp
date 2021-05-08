#include "AHRSManager.h"
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include "SBRCommon.h"
#include "sensors.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

using namespace sbrcontroller::sensors;
using namespace std;

namespace sbrcontroller {
    namespace ahrs {

        struct RegisterEntry
        {
            RegisterEntry() : killSignal(ATOMIC_VAR_INIT(false)) {}

            std::atomic_bool killSignal;
            std::thread publishThread;
        }

        AHRSManager::AHRSManager(std::shared_ptr<algorithms::IAHRSFusionAlgorithm> fusionAlgorithm,
            const std::vector<std::shared_ptr<ISensor>>& sensors,
            int sensorSamplePeriodHz,
            std::shared_ptr<spdlog::logger> pLogger) :
                m_pFusionAlgorithm(fusionAlgorithm),
                m_nSensorSamplePeriodHz(sensorSamplePeriodHz),
                m_pLogger(pLogger),
                m_bKillSignal(ATOMIC_VAR_INIT(false))
        {
            for (auto&& sensor : sensors) {
                auto sensorInfo = sensor->GetDeviceInfo();
                switch (sensorInfo.sensorType) {
                    case ESensorType::Gyroscope: m_pGyroSensor = sensor; break;
                    case ESensorType::Accelerometer: m_pAccelSensor = sensor; break;
                    case ESensorType::Magnetometer: m_pMagSensor = sensor; break;
                };
            }
            // TODO: If there is hardware assisted fusion
            if (m_pGyroSensor == nullptr && m_pAccelSensor == nullptr) {
                throw std::runtime_error("At least a gyroscope and an accelerometer sensor is required for ahrs sensor fusion");
            }

            // kick off the sampling thread
            m_tSensorFusionThread = std::thread([this] {SensorFusionThreadProc();});
        }

        AHRSManager::~AHRSManager()
        {
            m_bKillSignal.store(true);
            m_tSensorFusionThread.join();
        }

        void AHRSManager::SensorFusionThreadProc()
        {
            SetRealtimePriority();

            TripleAxisData gyroDataRadsPerSec = {}, accelDataGsPerSec = {}, magDataGauss = {};

            long sleepUS = 1000000 / m_nSensorSamplePeriodHz;
            m_pLogger->info("Starting ahrs sampling at {}Hz ({} us sleeps)", m_nSensorSamplePeriodHz, sleepUS);

            while (!m_bKillSignal.load()) 
            {
                auto sensorLoopBegin = std::chrono::high_resolution_clock::now();
    
                if (m_pGyroSensor->ReadSensorData(reinterpret_cast<unsigned char*>(&gyroDataRadsPerSec), sizeof(TripleAxisData)) != sizeof(TripleAxisData))
                        throw errorhandling::InvalidOperationException("Could not read gyro data");

                if (m_pAccelSensor->ReadSensorData(reinterpret_cast<unsigned char*>(&accelDataGsPerSec), sizeof(TripleAxisData)) != sizeof(TripleAxisData))
                        throw errorhandling::InvalidOperationException("Could not read accel data");

                if (m_pMagSensor != nullptr) {
                    if (m_pMagSensor->ReadSensorData(reinterpret_cast<unsigned char*>(&magDataGauss), sizeof(TripleAxisData)) != sizeof(TripleAxisData))
                        throw errorhandling::InvalidOperationException("Could not read mag data");
                    
                    m_pFusionAlgorithm->Update(gyroDataRadsPerSec, accelDataGsPerSec, magDataGauss);
                } else {
                    m_pFusionAlgorithm->UpdateIMU(gyroDataRadsPerSec, accelDataGsPerSec);
                }

                auto sensorReadEnd = std::chrono::high_resolution_clock::now();
                auto elapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(sensorReadEnd - sensorLoopBegin);

                m_pLogger->trace("Sensor read and ahrs calcs time {}us", elapsedUS.count());
                
                // could use platform independent sleep here instead, eg
                //std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS - (int)elapsed.count()));
                usleep(sleepUS - elapsedUS.count());

                auto sensorLoopEnd = std::chrono::high_resolution_clock::now();
                auto totalElapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(sensorLoopEnd - sensorLoopBegin);
                m_pLogger->trace("Sensor loop total time {}us", totalElapsedUS.count());
            }
        }

        void AHRSManager::Register(const std::string& channel, std::weak_ptr<IAHRSDataSubscriber> pSubscriber, int updateDeltaMS)
        {
            auto updateEntry = std::make_shared<RegisterEntry>();
            
            updateEntry->publishThread = std::thread([updateDeltaMS, pSubscriber, channel, updateEntry] {
                bool bNeedUnregister = false;
                while (!updateEntry->killSignal.load())
                {
                    auto updateLoopBegin = std::chrono::high_resolution_clock::now();

                    // read the current orientation
                    auto qfuture = m_pFusionAlgorithm->ReadFusedSensorDataAsync();

                    // lock the subscriber
                    if (auto pSubSP = pSubscriber.lock()) 
                    {
                        try 
                        {
                            pSubSP->OnUpdate(qfuture.get());
                        }
                        catch (const std::exception& e)
                        {
                            // we have ungracefully exited update handler due to an exception - log it
                            m_pLogger->error(ex.what());
                            // could additionally set bNeedUnregister = true here and break, but might as well keep trying
                        }
                    }
                    else 
                    {
                        // subscriber is no longer present, need to unregister
                        bNeedUnregister = true;
                        /////////////updateEntry->killSignal.store(true);////////////  !!! Thjink carefully about this - 
                        // order of destruction and unregistyering may matter (deadloclking etc)
                    }

                    auto updateLoopEnd = std::chrono::high_resolution_clock::now();
                    auto elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(updateLoopEnd - updateLoopBegin);
                    
                    std::this_thread::sleep_for(std::chrono::milliseconds(updateDeltaMS - (int)elapsedMS.count()));
                }
                if (bNeedUnregister)
                {
                    Unregister(channel);
                }
            });

            
            {// lock the registry map
                const std::lock_guard<std::mutex> lock(m_updateRegistryLock);
                m_updateRegistry[channel] = updateEntry;
            }// and release
        }

        void AHRSManager::Unregister(const std::string& channel)
        {
            //m_updateRegistry  // look up entry, signal kill and join thread here
            {// lock the registry map
                const std::lock_guard<std::mutex> lock(m_updateRegistryLock);
                m_updateRegistry.erase(channel); // remove from registry
            }// and release
        }

        void AHRSManager::SetRealtimePriority()
        {
            int ret;
        
            // We'll operate on the currently running thread.
            pthread_t this_thread = pthread_self();
            // struct sched_param is used to store the scheduling priority
            struct sched_param params;
        
            // We'll set the priority to the maximum.
            params.sched_priority = sched_get_priority_max(SCHED_FIFO);

            m_pLogger->debug("Trying to set thread realtime prio = {}", params.sched_priority);
        
            // Attempt to set thread real-time priority to the SCHED_FIFO policy
            ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
            if (ret != 0) {
                // Print the error
                m_pLogger->error("Unsuccessful in setting thread realtime prio");
                return;     
            }

            // Now verify the change in thread priority
            int policy = 0;
            ret = pthread_getschedparam(this_thread, &policy, &params);
            if (ret != 0) {
                m_pLogger->error("Couldn't retrieve real-time scheduling paramers");
                return;
            }
        
            // Check the correct policy was applied
            if(policy != SCHED_FIFO) {
                m_pLogger->error("Scheduling is NOT SCHED_FIFO!");
            } else {
                m_pLogger->debug("SCHED_FIFO OK");
            }
        
            // Print thread scheduling priority
            m_pLogger->debug("Thread priority is {}", params.sched_priority);
        }
    }
}