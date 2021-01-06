#pragma once

#include "ISpacialSensor.h"

namespace sbr {

    /***
     * Provides low level access to the MPU6050 6 axis gyroscope IC over an
     * I2C bus, and returns high level X/Y axis orientation data
     * Refer to document RM-MPU-6000A-00 rev 4.2 [RegisterMap] for info about
     * how the registers work.
     */ 
    class MPU6050 : public ISpacialSensor 
    {
        public:
            MPU6050();
            virtual ~MPU6050();

            virtual std::pair<double, double> ReadOrientation() override;

        private:
            unsigned short ReadHighLowI2CRegisters(int highRegister);
            double Dist(double a, double b);
            double RadsToDegrees(double radians);

            int m_fdMPU6050;
            int m_nCountsPerG;

            // primary I2C address
            static const int MPU6050_addr = 0x68;

            // registers
            static const int PWR_MGMT_1 = 0x6B;
            static const int ACCEL_CONFIG = 0x1C;
            static const int ACCEL_XOUT_H = 0x3B;
            static const int ACCEL_YOUT_H = 0x3D;
            static const int ACCEL_ZOUT_H = 0x3F;

            // data maps
            static const int ACCEL_FS_COUNTS[];
    };
}