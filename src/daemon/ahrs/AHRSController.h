#pragma once
#include "AHRS.h"
#include <memory>

namespace sbrcontroller
{
    namespace ahrs
    {

        class AHRSController : public IAHRSDataSource
        {
        public:
            AHRSController(std::shared_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
                std::shared_ptr<ISensor> gyro, 
                std::shared_ptr<ISensor> accel,
                int sensorSamplePeriodHz);
            AHRSController(std::shared_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
                std::shared_ptr<ISensor> gyro, 
                std::shared_ptr<ISensor> accel,
                std::shared_ptr<ISensor> mag,
                int sensorSamplePeriodHz);
            virtual ~AHRSController();

            virtual Ori3DRads ReadOrientation() override;

        private:
            void SensorFusionThreadProc();

            std::thread m_tSensorFusionThread;

            std::shared_ptr<IAHRSFusionAlgorithm> m_pFusionAlgorithm;
            std::shared_ptr<ISensor> m_pGyroSensor; 
            std::shared_ptr<ISensor> m_pAccelSensor;
            std::shared_ptr<ISensor> m_pMagSensor;
            int m_nSensorSamplePeriodHz;
            volatile bool m_bKillSignal;
        }

    }
}