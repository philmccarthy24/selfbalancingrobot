#pragma once

#include <utility>
#include <string>
#include <future>

namespace sbrcontroller
{
    namespace ahrs
    {
        class UnitConvertor 
        {
        public:
            static float ToDegrees(float radians);
            static float ToRadians(float degrees);
        };

        struct Ori3DRads 
        {
            float roll;
            float pitch;
            float yaw;
            float GetRollInDegrees();
            float GetPitchInDegrees();
            float GetYawInDegrees();
        };

        struct Quarternion {
            float w;
            float x;
            float y;
            float z;
            Ori3DRads ToEuler();
        };

        class IAHRSDataSource
        {
        public:
            IAHRSDataSource() {}
            virtual ~IAHRSDataSource() {}
            virtual Ori3DRads ReadOrientation() = 0;
        };

        class IAHRSFusionAlgorithm
        {
        public:
            IAHRSFusionAlgorithm() {}
            virtual ~IAHRSFusionAlgorithm() {}
            virtual bool IsHardwareImplementation() = 0;
            virtual void Update(const Axis3DSensorData& gyroData, const Axis3DSensorData& accelData, const Axis3DSensorData& magData) = 0;
            virtual void UpdateIMU(const Axis3DSensorData& gyroData, const Axis3DSensorData& accelData) = 0;

            virtual std::future<Quarternion> ReadFusedSensorDataAsync() = 0;
        };

        enum ESensorType {
            Gyroscope,
            Accelerometer,
            Magnetometor
        };

        class ISensor
        {
        public:
            ISensor() {}
            virtual ~ISensor() {}
            virtual ESensorType GetType() = 0;
            virtual std::string GetDeviceInfo() = 0;
            virtual SensorData GetData() = 0;
        };

        struct SensorData
        {
            SensorData() {}
            virtual ~SensorData() {}
        };

        struct Axis3DSensorData : public SensorData 
        {
            float x;
            float y;
            float z;
        };

        // In terms of design, there should be an AHRSController that has an internal thread,
        // responsible for getting raw data from 6/9 dof sensors and fusing them via a
        // particular algorithm to get orientation, and then asynchronously providing orientation 
        // readings to onwards processing (via promises as a clean simple thread-shared data mech.)
        // The AHRSController should manage 2 or 3 ISensors created via a factory (ie should
        // abstract the calling interface). How to handle the common MPU6050 (or other) setup code?
        

    }
}