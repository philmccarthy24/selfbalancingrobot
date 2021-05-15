#pragma once
#include "AHRS.h"
#include "IAHRSFusionAlgorithm.h"
#include <memory>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

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

        struct RegisterEntry;

        class AHRSManager : public IAHRSDataSource
        {
        public:
            AHRSManager(std::shared_ptr<algorithms::IAHRSFusionAlgorithm> fusionAlgorithm,
                const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors,
                int sensorSamplePeriodHz,
                std::shared_ptr<spdlog::logger> pLogger);
            virtual ~AHRSManager();

            virtual void Register(const std::string& channel, std::weak_ptr<IAHRSDataSubscriber> pSubscriber, int updateDeltaMS) override;
            virtual void Unregister(const std::string& channel) override;

        private:
            void SensorFusionThreadProc();
            void PublishThreadProc(std::shared_ptr<RegisterEntry> updateEntry, const std::string& channel, std::weak_ptr<IAHRSDataSubscriber> pSubscriber, int updateDeltaMS);

            std::thread m_tSensorFusionThread;

            std::mutex m_updateRegistryLock;
            std::map<std::string, std::shared_ptr<RegisterEntry>> m_updateRegistry;

            std::shared_ptr<algorithms::IAHRSFusionAlgorithm> m_pFusionAlgorithm;
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pGyroSensor; 
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pAccelSensor;
            std::shared_ptr<sbrcontroller::sensors::ISensor> m_pMagSensor;
            int m_nSensorSamplePeriodHz;
            std::atomic_bool m_bKillSignal;

            std::shared_ptr<spdlog::logger> m_pLogger;
        };

    }
}