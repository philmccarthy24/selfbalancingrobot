#pragma once

#include "AHRS.h"
#include "sensors.h"
#include <map>

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
             * Provides raw sensor readings over I2C bus from FXOS8700 Accelerometer
             * Programming example:
             * https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.h
             * https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.cpp
             * 
             */ 
            class FXOS8700Accel : public sbrcontroller::sensors::ISensor 
            {
                public:
                    enum EAccelRange {
                        ACCEL_RANGE_2G = 0x00, /**< +/- 2g range */
                        ACCEL_RANGE_4G = 0x01, /**< +/- 4g range */
                        ACCEL_RANGE_8G = 0x02  /**< +/- 8g range */
                    };

                    static const int I2C_ADDR = 0x1F; // Precision Magnetometer and Accelerometer IC

                    FXOS8700Accel(std::shared_ptr<coms::II2CDevice> pI2CDevice, std::shared_ptr<spdlog::logger> pLogger, EAccelRange accelRange = ACCEL_RANGE_2G);
                    virtual ~FXOS8700Accel();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;
                    virtual void ClearCalibration() override {}

                private:
                    std::shared_ptr<coms::II2CDevice> m_pFXOS8700;
                    std::shared_ptr<spdlog::logger> m_pLogger;

                    EAccelRange m_currentRange;

                    // registers
                    static const int FXOS8700_REGISTER_STATUS = 0x00;    /**< 0x00 */
                    static const int FXOS8700_REGISTER_OUT_X_MSB = 0x01; /**< 0x01 */
                    static const int FXOS8700_REGISTER_OUT_X_LSB = 0x02; /**< 0x02 */
                    static const int FXOS8700_REGISTER_OUT_Y_MSB = 0x03; /**< 0x03 */
                    static const int FXOS8700_REGISTER_OUT_Y_LSB = 0x04; /**< 0x04 */
                    static const int FXOS8700_REGISTER_OUT_Z_MSB = 0x05; /**< 0x05 */
                    static const int FXOS8700_REGISTER_OUT_Z_LSB = 0x06; /**< 0x06 */
                    static const int FXOS8700_REGISTER_WHO_AM_I = 0x0D; /**< 0x0D (default value = 0b11000111, read only) */
                    static const int FXOS8700_REGISTER_XYZ_DATA_CFG = 0x0E; /**< 0x0E */
                    static const int FXOS8700_REGISTER_CTRL_REG1 = 0x2A; /**< 0x2A (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_CTRL_REG2 = 0x2B; /**< 0x2B (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_CTRL_REG3 = 0x2C; /**< 0x2C (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_CTRL_REG4 = 0x2D; /**< 0x2D (default value = 0b00000000, read/write) */
                    static const int FXOS8700_REGISTER_CTRL_REG5 = 0x2E; /**< 0x2E (default value = 0b00000000, read/write) */

                    //static const float SENSORS_GRAVITY_STANDARD = 9.80665F;
                    // I think we want to return in Gs not ms-1?
                    
                    // Device ID for this sensor (used as sanity check during init)
                    static const int FXOS8700_ID = 0xC7; // 1100 0111

                    static const std::map<EAccelRange, float> m_RangeSensitivities;
            };
        }
    }
}