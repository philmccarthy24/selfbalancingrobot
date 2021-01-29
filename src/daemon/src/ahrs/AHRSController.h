#pragma once
#include "AHRS.h"
#include <memory>
#include <vector>

namespace sbrcontroller
{
    namespace ahrs
    {
        class ISensor;

        class AHRSController : public IAHRSDataSource
        {
        public:
            AHRSController(std::shared_ptr<IAHRSFusionAlgorithm> fusionAlgorithm,
                const std::vector<std::shared_ptr<ISensor>>& sensors,
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