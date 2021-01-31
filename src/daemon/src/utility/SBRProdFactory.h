#pragma once
#include "ISBRFactory.h"

namespace sbrcontroller {
    namespace utility {

        class SBRProdFactory : public ISBRFactory
        {
        public:
            SBRProdFactory();
            virtual ~SBRProdFactory();

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) override;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() override;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() override;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(const std::string& config) override;

        private:
        };
    }
}