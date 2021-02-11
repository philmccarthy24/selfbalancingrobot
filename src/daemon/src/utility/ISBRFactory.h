#pragma once

#include <memory>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {

    namespace sensors {
        class ISensor;
    }

    namespace coms {
        class II2CDevice;
    }

    namespace ahrs {
        class IAHRSDataSource;
        namespace algorithms {
            class IAHRSFusionAlgorithm;
        }
    }

    namespace utility {
        // forward class defs for better decoupling

        class ISBRFactory
        {
        public:
            ISBRFactory() {}
            virtual ~ISBRFactory() {}

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) const = 0;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() const = 0;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() const = 0;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(const std::string& config) const = 0;

            //... add more here. will need motor controller abstraction, etc
        };
    }
}