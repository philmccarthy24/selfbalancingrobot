#pragma once

#include <memory>

namespace sbrcontroller {
    namespace utility {

        // forward class defs for better decoupling
        class IConfigSection;
        namespace coms {
            class II2CDevice;
        }
        namespace ahrs {
            class IAHRSDataSource;
            class ISensor;
            namespace algorithms {
                class IAHRSFusionAlgorithm;
            }
        }

        class ISBRFactory
        {
        public:
            ISBRFactory();
            virtual ~ISBRFactory();

            virtual std::shared_ptr<IConfigSection> CreateConfig() = 0;
            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) = 0;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource(const std::string& config) = 0;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm(const std::string& config) = 0;
            virtual std::shared_ptr<ahrs::ISensor> CreateSensor(const std::string& config) = 0;

            //... add more here. will need motor controller abstraction, logging, etc
        };
    }
}