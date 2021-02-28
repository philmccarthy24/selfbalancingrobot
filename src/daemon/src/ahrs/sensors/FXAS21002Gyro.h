#pragma once
#include "AHRS.h"
#include "sensors.h"
#include <map>
#include <memory>

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
             * Provides low level access to the FXAS2100Gyro  gyroscope IC over an
             * I2C bus, and returns sensor data converted from raw counts to SI units.
             * Useful links for programming:
             * https://learn.adafruit.com/nxp-precision-9dof-breakout/arduino-code
             * https://github.com/adafruit/Adafruit_FXAS21002C/blob/master/Adafruit_FXAS21002C.h
             * https://github.com/adafruit/Adafruit_FXAS21002C/blob/master/Adafruit_FXAS21002C.cpp
             * 
             */ 
            class FXAS2100Gyro : public sbrcontroller::sensors::ISensor 
            {
                public:
                    enum ERange {
                        GYRO_RANGE_250DPS = 250,   /**< 250dps */
                        GYRO_RANGE_500DPS = 500,   /**< 500dps */
                        GYRO_RANGE_1000DPS = 1000, /**< 1000dps */
                        GYRO_RANGE_2000DPS = 2000  /**< 2000dps */
                    };

                    static const int I2C_ADDR = 0x21; // Precision Gyroscope IC

                    FXAS2100Gyro(std::shared_ptr<coms::II2CDevice> pI2CDevice, std::shared_ptr<spdlog::logger> pLogger, ERange range = GYRO_RANGE_250DPS);
                    virtual ~FXAS2100Gyro();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pFXAS2100;
                    float m_fCountsPerDegreesPerSec;
                    std::shared_ptr<spdlog::logger> m_pLogger;

                    ERange m_currentRange;

                    // registers
                    static const int GYRO_REGISTER_STATUS = 0x00;    /**< 0x00 */
                    static const int GYRO_REGISTER_OUT_X_MSB = 0x01; /**< 0x01 */
                    static const int GYRO_REGISTER_OUT_X_LSB = 0x02; /**< 0x02 */
                    static const int GYRO_REGISTER_OUT_Y_MSB = 0x03; /**< 0x03 */
                    static const int GYRO_REGISTER_OUT_Y_LSB = 0x04; /**< 0x04 */
                    static const int GYRO_REGISTER_OUT_Z_MSB = 0x05; /**< 0x05 */
                    static const int GYRO_REGISTER_OUT_Z_LSB = 0x06; /**< 0x06 */
                    static const int GYRO_REGISTER_WHO_AM_I = 0x0C; /**< 0x0C (default value = 0b11010111, read only) */
                    static const int GYRO_REGISTER_CTRL_REG0 = 0x0D; /**< 0x0D (default value = 0b00000000, read/write) */
                    static const int GYRO_REGISTER_CTRL_REG1 = 0x13; /**< 0x13 (default value = 0b00000000, read/write) */
                    static const int GYRO_REGISTER_CTRL_REG2 = 0x14; /**< 0x14 (default value = 0b00000000, read/write) */

                    // data maps
                    static const std::map<ERange, float> m_RangeSensitivities;

                    // Device ID for this sensor (used as a sanity check during init)
                    static const int FXAS21002C_ID = 0xD7; // 1101 0111

            };
        }
    }
}