#pragma once

#include <memory>
#include "ISpacialSensor.h"

namespace sbrcontroller
{
    namespace imu 
    {
        class MPU6050I2C;

        class MPU6050Gyro : ISpacialSensor 
        {
        public:
        // going to need the accelerometer for initial reading - weak ptr?
            MPU6050Gyro(std::shared<MPU6050I2C> sensorComs, std::weak_ptr<ISpacialSensor> initialReferenceSensor);
            virtual ~MPU6050Gyro();
            virtual Orientation3D ReadOrientation() override;

        private:
            int64_t m_tsOrientationLastRead;
            Orientation3D m_lastReading;

            double m_dbDegreesPerSec;

            std::shared<MPU6050I2C> m_pSensorComs;
            std::weak_ptr<ISpacialSensor> m_pInitialReferenceSensor;

            // data maps
            static const double GYRO_FS_COUNTS[];
        };

    }
}


