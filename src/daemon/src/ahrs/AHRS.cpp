#include "AHRS.h"
#include <cmath>

namespace sbrcontroller
{
    namespace ahrs
    {

        float UnitConvertor::ToDegrees(float radians)
        {
            return (radians * 180.0) / M_PI;
        }

        float UnitConvertor::ToRadians(float degrees)
        {
            return (degrees * M_PI) / 180;
        }

        float Ori3DRads::GetRollInDegrees() const
        {
            return UnitConvertor::ToDegrees(roll);
        }

        float Ori3DRads::GetPitchInDegrees() const
        {
            return UnitConvertor::ToDegrees(pitch);
        }
            
        float Ori3DRads::GetYawInDegrees() const
        {
            return UnitConvertor::ToDegrees(yaw);
        }

        Ori3DRads Quaternion::ToEuler() 
        {
            // convert Quaternion to orientation (euler angles)
            Ori3DRads angles;

            angles.roll = atan2f(w*x + y*z, 0.5f - x*x - y*y);
            angles.pitch = asinf(-2.0f * (x*z - w*y));
            angles.yaw = atan2f(x*y + w*z, 0.5f - y*y - z*z);

/*
            // roll (x-axis rotation)
            double sinr_cosp = 2 * (w * x + y * z);
            double cosr_cosp = 1 - 2 * (x * x + y * y);
            angles.roll = std::atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            double sinp = 2 * (w * y - z * x);
            if (std::abs(sinp) >= 1)
                angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
            else
                angles.pitch = std::asin(sinp);

            // yaw (z-axis rotation)
            double siny_cosp = 2 * (w * z + x * y);
            double cosy_cosp = 1 - 2 * (y * y + z * z);
            angles.yaw = std::atan2(siny_cosp, cosy_cosp);
*/

            return angles;
        }

    }
}