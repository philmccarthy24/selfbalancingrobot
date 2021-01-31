#pragma once

#include "sensors.h"
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
            virtual void Update(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData, const sensors::TripleAxisData& magData) = 0;
            virtual void UpdateIMU(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData) = 0;

            virtual std::future<Quarternion> ReadFusedSensorDataAsync() = 0;
        };

    }
}