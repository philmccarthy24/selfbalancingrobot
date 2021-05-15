#include "AHRSManager.h"
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include "SBRCommon.h"
#include "sensors.h"
#include "ThreadHelper.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <unistd.h>
#include <pthread.h>

using namespace sbrcontroller::sensors;
using namespace std;

namespace sbrcontroller {
    namespace ahrs {

        struct RegisterEntry
        {
            RegisterEntry() : killSignal(ATOMIC_VAR_INIT(false)) {}

            std::atomic_bool killSignal;
            std::thread publishThread;
        };

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
            // unregister all the subscribers and clean up their threads
            std::vector<std::string> channels;
            {// lock the registry map
                const std::lock_guard<std::mutex> lock(m_updateRegistryLock);

                for (auto updateEntryIter : m_updateRegistry)
                {
                    channels.push_back(updateEntryIter.first);
                }
            }
            for (const auto& channel : channels)
            {
                Unregister(channel);
            }

            // now we can kill sensor fusion thread
            m_bKillSignal.store(true);
            m_tSensorFusionThread.join();
        }

        void AHRSManager::SensorFusionThreadProc()
        {
            utility::ThreadHelper::SetRealtimePriority(m_pLogger);

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
            
            updateEntry->publishThread = std::thread([this, updateDeltaMS, pSubscriber, channel, updateEntry] {
                PublishThreadProc(updateEntry, channel, pSubscriber, updateDeltaMS);
            });
            
            {// lock the registry map
                const std::lock_guard<std::mutex> lock(m_updateRegistryLock);
                m_updateRegistry[channel] = updateEntry;
            }// and release
        }

        void AHRSManager::Unregister(const std::string& channel)
        {
            {// lock the registry map
                const std::lock_guard<std::mutex> lock(m_updateRegistryLock);

                std::shared_ptr<RegisterEntry> updateEntry = nullptr;
            
                auto updateEntryIter = m_updateRegistry.find(channel);
                if (updateEntryIter != m_updateRegistry.end()) {
                    updateEntry = updateEntryIter->second;
                }

                if (updateEntry != nullptr)
                {
                    // signal the publish thread to end
                    updateEntry->killSignal.store(true);
                    // wait for it to terminate
                    updateEntry->publishThread.join();

                    m_updateRegistry.erase(channel); // remove from registry
                } // otherwise, has already been unregistered

            }// and release
        }

        void AHRSManager::PublishThreadProc(std::shared_ptr<RegisterEntry> updateEntry, const std::string& channel, std::weak_ptr<IAHRSDataSubscriber> pSubscriber, int updateDeltaMS)
        {
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
                    catch (const std::exception& ex)
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
                    break;
                }

                auto updateLoopEnd = std::chrono::high_resolution_clock::now();
                auto elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(updateLoopEnd - updateLoopBegin);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(updateDeltaMS - (int)elapsedMS.count()));
            }
            if (bNeedUnregister)
            {
                Unregister(channel);
            }
        }
        
    }
}