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

        struct Quaternion {
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
            virtual void Register(const std::string& channel, std::weak_ptr<IAHRSDataSubscriber> pSubscriber, int updateDeltaMS) = 0;
            virtual void Unregister(const std::string& channel) = 0;
        };

        class IAHRSDataSubscriber
        {
            public:
            IAHRSDataSubscriber() {}
            virtual ~IAHRSDataSubscriber() {}

            virtual void OnUpdate(const Quaternion& orientation) = 0;
        }
    }
}