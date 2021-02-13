#pragma once
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include <memory>
#include <vector>
#include <thread>

namespace spdlog {
    class logger;
}

namespace sbrcontroller
{
    namespace sensors {
        class ISensor;
    }

    namespace ahrs
    {
        namespace algorithms {
            class IAHRSFusionAlgorithm;
        }

        class AHRSController : public IAHRSDataSource
        {
        public:
            AHRSController(std::shared_ptr<algorithms::IAHRSFusionAlgorithm> fusionAlgorithm,
                const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors,
                int sensorSamplePeriodHz,
                std::shared_ptr<spdlog::logger> pLogger);
            virtual ~AHRSController();

            virtual Ori3DRads ReadOrientation() override;

        private:
            void SensorFusionThreadProc();

            std::thread m_tSensorFusionThread;

            std::shared_ptr<algorithms::IAHRSFusionAlgorithm> m_pFusionAlgorithm;
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pGyroSensor; 
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pAccelSensor;
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pMagSensor;
            int m_nSensorSamplePeriodHz;
            volatile bool m_bKillSignal;

            std::shared_ptr<spdlog::logger> m_pLogger;
        };

    }
}