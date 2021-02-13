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
            float GetRollInDegrees() const;
            float GetPitchInDegrees() const;
            float GetYawInDegrees() const;
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

    }
}