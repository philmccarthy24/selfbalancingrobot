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
        class ISerialDevice;
        class IStringReaderWriter;
    }

    namespace motor {
        class IMotorController;
    }

    namespace ahrs {
        class IAHRSDataSource;
        namespace algorithms {
            class IAHRSFusionAlgorithm;
        }
    }

    namespace utility {
        // forward class defs for better decoupling

        class IChecksumCalculator;
        class IConfigSection;

        class ISBRFactory
        {
        public:
            ISBRFactory() {}
            virtual ~ISBRFactory() {}

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) const = 0;
            virtual std::shared_ptr<coms::ISerialDevice> CreateSerialDevice(const std::string& serialDeviceName, int baudRate) const = 0;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() const = 0;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() const = 0;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(std::shared_ptr<sbrcontroller::utility::IConfigSection> sensorConfig) const = 0;

            //... add more here. will need motor controller abstraction, etc
            virtual std::shared_ptr<coms::IStringReaderWriter> CreateStringReaderWriter(std::shared_ptr<coms::ISerialDevice> pSerialDevice) const = 0;
            virtual std::shared_ptr<IChecksumCalculator> CreateChecksumCalculator() const = 0;
            virtual std::shared_ptr<motor::IMotorController> CreateMotorController() const = 0;
        };
    }
}