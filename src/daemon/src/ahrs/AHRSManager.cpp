#include "AHRSManager.h"
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include "sbrcontroller.h"
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
                
                //std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS - (int)elapsed.count()));
                usleep(sleepUS - elapsedUS.count());

                auto sensorLoopEnd = std::chrono::high_resolution_clock::now();
                auto  totalElapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(sensorLoopEnd - sensorLoopBegin);
                m_pLogger->trace("Sensor loop total time {}us", totalElapsedUS.count());
            }
        }

        Quaternion AHRSManager::ReadOrientation()
        {
            auto qfuture = m_pFusionAlgorithm->ReadFusedSensorDataAsync();
            return qfuture.get(); // wait for the promise to be fulfilled
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