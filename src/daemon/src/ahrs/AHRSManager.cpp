#include "AHRSManager.h"
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include "sbrcontroller.h"
#include "sensors.h"
#include "spdlog/spdlog.h"

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
            TripleAxisData gyroDataRadsPerSec, accelDataGsPerSec, magDataGauss;

            int sleepMS = 1000 / m_nSensorSamplePeriodHz;
            m_pLogger->info("Starting ahrs sampling at {}Hz ({} ms sleeps)", m_nSensorSamplePeriodHz, sleepMS);

            while (!m_bKillSignal.load()) 
            {

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
                
                // this won't be massively accurate unfortunately (TODO: add metrics)
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS));
            }
        }

        Ori3DRads AHRSManager::ReadOrientation()
        {
            auto qfuture = m_pFusionAlgorithm->ReadFusedSensorDataAsync();
            auto q = qfuture.get(); // wait for the promise to be fulfilled
            return q.ToEuler();
        }
    }
}