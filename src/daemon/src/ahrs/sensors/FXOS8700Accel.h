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
             * Provides raw sensor readings over I2C bus from FXOS8700 Accelerometer
             * Programming example:
             * https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.h
             * https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.cpp
             * 
             */ 
            class FXOS8700Accel : public sbrcontroller::sensors::ISensor 
            {
                public:
                    static const int I2C_ADDR = 0x1F; // Precision Magnetometer and Accelerometer IC

                    FXOS8700Accel(std::shared_ptr<coms::II2CDevice> pI2CDevice);
                    virtual ~FXOS8700Accel();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pMPU6050;
                    int m_nCountsPerG;

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