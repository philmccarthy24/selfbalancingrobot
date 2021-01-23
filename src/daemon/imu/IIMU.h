#pragma once

#include <utility>

namespace sbrcontroller
{
    namespace imu 
    {

        struct Orientation3D {
            double m_dbRoll_deg;
            double m_dbPitch_deg;
            double m_dbYaw_deg;
        };

        class IIMU 
        {
        public:
            IIMU() {}
            virtual ~IIMU() {}
            virtual Orientation3D ReadOrientation() = 0;
        };

    }
}