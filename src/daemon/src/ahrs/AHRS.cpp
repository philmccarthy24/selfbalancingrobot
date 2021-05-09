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

        Ori3DRads Quaternion::ToEuler() const
        {
            // convert Quaternion to orientation (euler angles)
            Ori3DRads angles;

            angles.roll = atan2f(w*x + y*z, 0.5f - x*x - y*y);
            angles.pitch = asinf(-2.0f * (x*z - w*y));
            angles.yaw = atan2f(x*y + w*z, 0.5f - y*y - z*z);

            return angles;
        }

    }
}