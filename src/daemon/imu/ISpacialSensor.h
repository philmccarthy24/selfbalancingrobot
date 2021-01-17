#pragma once

#include <utility>

namespace sbrcontroller
{
    namespace imu 
    {

        struct Orientation3D {
            double m_dbPitch_deg;
            double m_dbRoll_deg;
            double m_dbYaw_deg;
        };

        class ISpacialSensor 
        {
        public:
            ISpacialSensor() {}
            virtual ~ISpacialSensor() {}
            virtual Orientation3D ReadOrientation() = 0;
        };

    }
}