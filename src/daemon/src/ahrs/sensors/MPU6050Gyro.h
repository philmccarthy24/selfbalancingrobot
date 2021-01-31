#pragma once
#include "AHRS.h"
#include "sensors.h"

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
             */ 
            class MPU6050Gyro : public sbrcontroller::sensors::ISensor 
            {
                public:
                    MPU6050Gyro(std::shared_ptr<coms::II2CDevice> pI2CDevice);
                    virtual ~MPU6050Gyro();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pMPU6050;
                    float m_fRadiansPerSec;

                    // registers
                    static const int PWR_MGMT_1 = 0x6B;
                    static const int GYRO_CONFIG = 0x1B;
                    static const int GYRO_XOUT_H = 0x43;
                    static const int GYRO_YOUT_H = 0x45;
                    static const int GYRO_ZOUT_H = 0x47;

                    // data maps
                    static const double GYRO_FS_COUNTS[];
            };
        }
    }
}