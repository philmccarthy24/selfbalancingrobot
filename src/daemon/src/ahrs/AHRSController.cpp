#include "AHRSController.h"
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include "sbrcontroller.h"
#include "sensors.h"

using namespace sbrcontroller::sensors;
using namespace std;

namespace sbrcontroller {
    namespace ahrs {

        AHRSController::AHRSController(std::shared_ptr<algorithms::IAHRSFusionAlgorithm> fusionAlgorithm,
            const std::vector<std::shared_ptr<ISensor>>& sensors,
            int sensorSamplePeriodHz) :
                m_pFusionAlgorithm(fusionAlgorithm),
                m_nSensorSamplePeriodHz(sensorSamplePeriodHz),
                m_bKillSignal(false)
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

        AHRSController::~AHRSController()
        {
            m_bKillSignal = true;
            m_tSensorFusionThread.join();
        }

        void AHRSController::SensorFusionThreadProc()
        {
            TripleAxisData gyroDataRadsPerSec, accelDataGsPerSec, magDataGauss;

            while (!m_bKillSignal) {

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

                // we sample at 50Hz, so wait 20ms between readings. this won't be
                // massively accurate unfortunately (TODO: add metrics)
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }

        Ori3DRads AHRSController::ReadOrientation()
        {
            auto qfuture = m_pFusionAlgorithm->ReadFusedSensorDataAsync();
            auto q = qfuture.get(); // wait for the promise to be fulfilled
            return q.ToEuler();
        }
    }
}