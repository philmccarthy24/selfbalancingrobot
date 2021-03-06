#pragma once

#include "AHRS.h"
#include "sensors.h"

namespace spdlog {
    class logger;
}

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
                    static const int I2C_ADDR = 0x1F; // Precision Magnetometer and Accelerometer IC

                    FXOS8700Mag(std::shared_ptr<coms::II2CDevice> pI2CDevice, std::shared_ptr<spdlog::logger> pLogger);
                    virtual ~FXOS8700Mag();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pFXOS8700;
                    std::shared_ptr<spdlog::logger> m_pLogger;

                    // registers
                    static const int FXOS8700_REGISTER_WHO_AM_I = 0x0D; /**< 0x0D (default value = 0b11000111, read only) */
                    static const int FXOS8700_REGISTER_MSTATUS = 0x32;    /**< 0x32 */
                    static const int FXOS8700_REGISTER_MOUT_X_MSB = 0x33; /**< 0x33 */
                    static const int FXOS8700_REGISTER_MOUT_X_LSB = 0x34; /**< 0x34 */
                    static const int FXOS8700_REGISTER_MOUT_Y_MSB = 0x35; /**< 0x35 */
                    static const int FXOS8700_REGISTER_MOUT_Y_LSB = 0x36; /**< 0x36 */
                    static const int FXOS8700_REGISTER_MOUT_Z_MSB = 0x37; /**< 0x37 */
                    static const int FXOS8700_REGISTER_MOUT_Z_LSB = 0x38; /**< 0x38 */
                    static const int FXOS8700_REGISTER_MCTRL_REG1 = 0x5B; /**< 0x5B (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_MCTRL_REG2 = 0x5C; /**< 0x5C (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_MCTRL_REG3 = 0x5D; /**< 0x5D (default value = 0b00000000, read/write) */

                    // Device ID for this sensor (used as sanity check during init)
                    static const int FXOS8700_ID = 0xC7; // 1100 0111

                    // micro tesla (uT) per LSB (1 LSB = 0.1uT)
                    static const float MAG_UT_LSB;
            };
        }
    }
}