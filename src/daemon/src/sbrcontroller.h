#pragma once
#include <string>
#include <stdexcept>

namespace sbrcontroller {

    // config keys
    static const std::string AHRS_FUSION_ALGORITHM_CONFIG_KEY = "/ahrs/fusionAlgorithm";
    static const std::string AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY = "/ahrs/sensorSampleRateHz";
    static const std::string AHRS_SENSOR_IDS_KEY = "/ahrs/sensors";

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
    }

}