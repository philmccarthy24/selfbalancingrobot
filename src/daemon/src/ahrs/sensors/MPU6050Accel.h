#pragma once
#include <thread>
#include "AHRS.h"

namespace sbrcontroller {

    namespace coms {
        class II2CDevice;
    }

    namespace ahrs {
        namespace sensors {

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
            class MPU6050Accel : public ISensor 
            {
                public:
                    MPU6050Accel();
                    virtual ~MPU6050Accel();

                    virtual ESensorType GetType() override;
                    virtual std::string GetDeviceInfo() override;
                    virtual SensorData GetData() override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pMPU6050;
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
    }
}