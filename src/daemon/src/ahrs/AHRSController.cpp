#include "AHRSController.h"
#include "AHRS.h"

namespace sbrcontroller {
    namespace ahrs {

        AHRSController::AHRSController(std::shared_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
            const std::vector<std::shared_ptr<ISensor>>& sensors,
            int sensorSamplePeriodHz) :
                m_pFusionAlgorithm(fusionAlgorithm),
                m_nSensorSamplePeriodHz(sensorSamplePeriodHz),
                m_bKillSignal(false)
        {
            for (auto&& sensor : sensors) {
                switch (sensor->GetType()) {
                    case ESensorType::Gyroscope: m_pGyrsoSensor = sensor; break;
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

        AHRSController::~AHRSController()
        {
            m_bKillSignal = true;
            m_tSensorFusionThread.join();
        }

        void AHRSController::SensorFusionThreadProc()
        {
            while (!m_bKillSignal) {

                auto gyroDataRadsPerSec = static_cast<Axis3DSensorData>(m_pGyroSensor->GetData());
                auto accelDataGsPerSec = static_cast<Axis3DSensorData>(m_pAccelSensor->GetData());
                if (m_pMagSensor != null) {
                    auto magData = static_cast<Axis3DSensorData>(m_pMagSensor->GetData());
                    m_pFusionAlgorithm->Update(gyroDataRadsPerSec, accelDataGsPerSec, magData);
                } else {
                    m_pFusionAlgorithm->UpdateIMU(gyroDataRadsPerSec, accelDataGsPerSec);
                }


                
                short gyXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_XOUT_H));
                short gyYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_YOUT_H));
                short gyZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_ZOUT_H));

                
                float gyX = static_cast<float>(gyXRawCounts) / static_cast<float>(m_fDegreesPerSec);
                float gyY = static_cast<float>(gyYRawCounts) / static_cast<float>(m_fDegreesPerSec);
                float gyZ = static_cast<float>(gyZRawCounts) / static_cast<float>(m_fDegreesPerSec);

                m_imuFilter.Update6dofIMU(DegreesToRadians(gyX), DegreesToRadians(gyY), DegreesToRadians(gyZ), accX, accY, accZ);

                // we sample at 50Hz, so wait 20ms between readings. this won't be
                // massively accurate unfortunately (TODO: add metrics)
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        Ori3DRads AHRSController::ReadOrientation()
        {
            auto qfuture = m_imuFilter.RequestSensorReading();
            auto q = qfuture.get(); // wait for the promise to be fulfilled

            
        }
    }
}