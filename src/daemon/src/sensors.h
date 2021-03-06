#pragma once
#include <string>

namespace sbrcontroller {
    namespace sensors {
    
        enum ESensorType {
            Gyroscope,
            Accelerometer,
            Magnetometer
        };

        enum ESensorDataType {
            TripleAxis_RotationalVelocity_RadiansPerSec,
            TripleAxis_Acceleration_G,
            TripleAxis_MagneticFieldStrength_MicroTesla
        };

        struct TripleAxisData
        {
            float x;
            float y;
            float z;
        };

        struct SensorInfo
        {
            std::string identifier;
            ESensorType sensorType;
            ESensorDataType dataType;
            int requiredDataLength;
        };

        class ISensor
        {
        public:
            ISensor() {}
            virtual ~ISensor() {}

            virtual SensorInfo GetDeviceInfo() = 0;
            virtual int ReadSensorData(unsigned char* buffer, unsigned int length) = 0;
            virtual void ClearCalibration() = 0;
        };
    }
}