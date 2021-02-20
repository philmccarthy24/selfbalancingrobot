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
             * Provides raw sensor readings over I2C bus from a FXOS8700 Magnetometer
             * 
             */ 
            class FXOS8700Mag : public sbrcontroller::sensors::ISensor 
            {
                public:
                    FXOS8700Accel(std::shared_ptr<coms::II2CDevice> pI2CDevice);
                    virtual ~FXOS8700Accel();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pFXOS8700;
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