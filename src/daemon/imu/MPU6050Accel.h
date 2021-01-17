#pragma once

#include <memory>
#include "ISpacialSensor.h"

namespace sbrcontroller
{
    namespace imu 
    {
        class MPU6050I2C;

        class MPU6050Accel : ISpacialSensor 
        {
        public:
            MPU6050Accel(std::shared<MPU6050I2C> pSensorComs);
            virtual ~MPU6050Accel();
            virtual Orientation3D ReadOrientation() override;

        private:
            double Dist(double a, double b);
            double RadsToDegrees(double radians);

            int m_nCountsPerG;

            std::shared<MPU6050I2C> m_pSensorComs;

            // data maps
            static const int ACCEL_FS_COUNTS[];
        };

    }
}

