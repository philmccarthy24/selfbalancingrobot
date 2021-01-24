#pragma once
#include "ISBRFactory.h"

namespace sbrcontroller {
    namespace utility {

        class SBRProdFactory : public ISBRFactory
        {
        public:
            SBRProdFactory(const std::string& configFilePath);
            virtual ~SBRProdFactory();

            virtual std::shared_ptr<IConfigSection> CreateConfig() override;
            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) override;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource(const std::string& config) override;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm(const std::string& config) override;
            virtual std::shared_ptr<ahrs::ISensor> CreateSensor(const std::string& config) override;

        private:
            std::string m_configFilePath;
        }
    }
}