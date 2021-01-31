#pragma once

#include <memory>

namespace sbrcontroller {

    namespace sensors {
        class ISensor;
    }

    namespace utility {
        // forward class defs for better decoupling
        namespace coms {
            class II2CDevice;
        }
        namespace ahrs {
            class IAHRSDataSource;
            namespace algorithms {
                class IAHRSFusionAlgorithm;
            }
        }

        class ISBRFactory
        {
        public:
            ISBRFactory();
            virtual ~ISBRFactory();

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) = 0;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() = 0;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() = 0;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(const std::string& config) = 0;

            //... add more here. will need motor controller abstraction, logging, etc
        };
    }
}