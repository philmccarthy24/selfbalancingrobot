#include "AHRSController.h"

namespace sbrcontroller {
    namespace ahrs {

        AHRSController::AHRSController(std::unique_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
            std::unique_ptr<ISensor> gyro, 
            std::unique_ptr<ISensor> accel,
            int sensorSamplePeriodHz) :
            AHRSController(fusionAlgorithm, gyro, accel, nullptr, sensorSamplePeriodHz)
        {
        }

        AHRSController::AHRSController(std::unique_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
            std::unique_ptr<ISensor> gyro, 
            std::unique_ptr<ISensor> accel,
            std::unique_ptr<ISensor> mag,
            int sensorSamplePeriodHz) :
                m_pFusionAlgorithm(std::move(fusionAlgorithm)),
                m_pGyroSensor(std::move(gyro)),
                m_pAccelSensor(std::move(accel)),
                m_pMagSensor(std::move(mag)),
                m_nSensorSamplePeriodHz(sensorSamplePeriodHz),
                m_bKillSignal(false)
        {
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
                // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
                short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
                short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
                short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));
                short gyXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_XOUT_H));
                short gyYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_YOUT_H));
                short gyZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_ZOUT_H));

                // convert to units
                float accX = static_cast<float>(accXRawCounts) / static_cast<float>(m_nCountsPerG);
                float accY = static_cast<float>(accYRawCounts) / static_cast<float>(m_nCountsPerG);
                float accZ = static_cast<float>(accZRawCounts) / static_cast<float>(m_nCountsPerG);
                float gyX = static_cast<float>(gyXRawCounts) / static_cast<float>(m_fDegreesPerSec);
                float gyY = static_cast<float>(gyYRawCounts) / static_cast<float>(m_fDegreesPerSec);
                float gyZ = static_cast<float>(gyZRawCounts) / static_cast<float>(m_fDegreesPerSec);

                m_imuFilter.Update6dofIMU(DegreesToRadians(gyX), DegreesToRadians(gyY), DegreesToRadians(gyZ), accX, accY, accZ);

                // we sample at 50Hz, so wait 20ms between readings. this won't be
                // massively accurate unfortunately (TODO: add metrics)
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
    }
}