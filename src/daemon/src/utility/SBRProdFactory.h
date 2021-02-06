#pragma once
#include "ISBRFactory.h"

namespace sbrcontroller {
    namespace utility {

        class SBRProdFactory : public ISBRFactory
        {
        public:
            SBRProdFactory();
            virtual ~SBRProdFactory();

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) const override;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() const override;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() const override;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(const std::string& config) const override;

            virtual std::shared_ptr<spdlog::logger> CreateLogger(const std::string& className) const override;

        private:
        };
    }
}