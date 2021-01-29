#include "AHRS.h"
#include <cmath>

namespace sbrcontroller
{
    namespace ahrs
    {

        float UnitConvertor::ToDegrees(float radians)
        {
            return radians * (180.0 / M_PI);
        }

        float UnitConvertor::ToRadians(float degrees)
        {
            return (degrees * M_PI) / 180;
        }

        float Ori3DRads::GetRollInDegrees()
        {
            return UnitConvertor::ToDegrees(roll);
        }

        float Ori3DRads::GetPitchInDegrees()
        {
            return UnitConvertor::ToDegrees(pitch);
        }
            
        float Ori3DRads::GetYawInDegrees()
        {
            return UnitConvertor::ToDegrees(yaw);
        }

        Ori3DRads Quarternion::ToEuler() 
        {
            // convert quarternion to orientation (euler angles)
            Ori3DRads angles;

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

            return angles;
        }

    }
}