#pragma once
#include <string>
#include <stdexcept>

namespace sbrcontroller {

    // config keys
    static const std::string AHRS_FUSION_ALGORITHM_CONFIG_KEY = "/ahrs/fusionAlgorithm";
    static const std::string AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY = "/ahrs/sensorSampleRateHz";
    static const std::string AHRS_SENSOR_IDS_KEY = "/ahrs/sensors";
    static const std::string LOGGING_LEVEL_KEY = "/logging/level";
    static const std::string LOGGING_PATTERN_KEY = "/logging/pattern";
    static const std::string LOGGING_SINKS_KEY = "/logging/sinks";
    static const std::string LOGGING_LOGGERS_KEY = "/logging/loggers";
    static const std::string MOTOR_CONTROL_COMS_KEY = "/motorControl/coms";

    // engineering constants
    static const int MPU6050_I2C_ADDR = 0x68;

    // exceptions
    namespace errorhandling {
        class NotImplementedException : public std::logic_error
        {
        public:
            NotImplementedException(const std::string& msg) : 
                std::logic_error(msg) 
            {
            }
        };

        class ComsException : public std::runtime_error
        {
        public:
            ComsException(const std::string& msg) : 
                std::runtime_error(msg) 
            {
            }
        };

        class InvalidOperationException : public std::runtime_error
        {
        public:
            InvalidOperationException(const std::string& msg) : 
                std::runtime_error(msg) 
            {
            }
        };

        class ParseException : public std::runtime_error
        {
        public:
            ParseException(const std::string& msg) : 
                std::runtime_error(msg) 
            {
            }
        };

        class ConfigurationException : public std::runtime_error
        {
        public:
            ConfigurationException(const std::string& msg) : 
                std::runtime_error(msg) 
            {
            }
        };
    }

}