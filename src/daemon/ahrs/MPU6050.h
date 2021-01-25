#pragma once
#include <thread>
#include "IIMU.h"
#include "Madgwick.h"

namespace sbrcontroller {
    namespace imu {

    /***
     * Provides low level access to the MPU6050 6 axis gyroscope IC over an
     * I2C bus, and returns sensor data converted from raw counts to SI units.
     * Refer to document RM-MPU-6000A-00 rev 4.2 [RegisterMap] for info about
     * how the registers work.
     * 
     * Note this code gets raw data from sensor registers. Using the DMP chip via 
     * the FIFO would be a better choice because it has processing/filtering built in 
     * (returning quarternions that can be converted to pitch/roll/yaw), however 
     * reading to a buffer over I2C using WiringPi isn't currently available.
     * Jeff Rowland's MPU6050/DMP code has been ported to older Pis via the 
     * bcm2835 lib, but the i2c lib that uses it (or bcm2835) is broken on Pi4.
     * 
     * This is an area to investigate, but to keep perfect being the enemy of good,
     * will use the raw readings along with a Madgwick filter (which has superior
     * accuracy and less computational overhead than a Kalman filter, see 
     * https://x-io.co.uk/open-source-imu-and-ahrs-algorithms/. This might well be
     * what the MP chip uses under the hood)
     */ 
    class MPU6050 : public IIMU 
    {
        public:
            MPU6050();
            virtual ~MPU6050();

            virtual Orientation3D ReadOrientation() override;

        private:
            unsigned short ReadHighLowI2CRegisters(int highRegister);
            void RawSampleUpdateThreadProc();
            inline double RadiansToDegrees(double radians);
            inline double DegreesToRadians(double degrees);

            int m_fdMPU6050;
            int m_nCountsPerG;
            float m_fDegreesPerSec;

            std::thread m_tSampleUpdateThread;
            Madgwick m_imuFilter;
            volatile bool m_bKillSignal;

            static const int SENSOR_SAMPLE_PERIOD_HZ = 20;

            // primary I2C address
            static const int MPU6050_addr = 0x68;

            // registers
            static const int PWR_MGMT_1 = 0x6B;
            static const int ACCEL_CONFIG = 0x1C;
            static const int GYRO_CONFIG = 0x1B;
            static const int ACCEL_XOUT_H = 0x3B;
            static const int ACCEL_YOUT_H = 0x3D;
            static const int ACCEL_ZOUT_H = 0x3F;
            static const int GYRO_XOUT_H = 0x43;
            static const int GYRO_YOUT_H = 0x45;
            static const int GYRO_ZOUT_H = 0x47;

            // data maps
            static const int ACCEL_FS_COUNTS[];
            static const double GYRO_FS_COUNTS[];
    };
    }
}