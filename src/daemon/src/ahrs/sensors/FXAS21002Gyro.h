#pragma once
#include "AHRS.h"
#include "sensors.h"
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
             * 
             */ 
            class FXAS2100Gyro : public sbrcontroller::sensors::ISensor 
            {
                public:
                    FXAS2100Gyro(std::shared_ptr<coms::II2CDevice> pI2CDevice, std::shared_ptr<spdlog::logger> pLogger);
                    virtual ~FXAS2100Gyro();

                    virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                    virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;

                private:
                    std::shared_ptr<coms::II2CDevice> m_pFXAS2100;
                    float m_fCountsPerDegreesPerSec;
                    std::shared_ptr<spdlog::logger> m_pLogger;

                    // registers
                    static const int PWR_MGMT_1 = 0x6B;
                    static const int GYRO_CONFIG = 0x1B;
                    static const int GYRO_XOUT_H = 0x43;
                    static const int GYRO_YOUT_H = 0x45;
                    static const int GYRO_ZOUT_H = 0x47;

                    // data maps
                    static const double GYRO_FS_COUNTS[];

https://learn.adafruit.com/nxp-precision-9dof-breakout/arduino-code
https://github.com/adafruit/Adafruit_FXAS21002C/blob/master/Adafruit_FXAS21002C.h
https://github.com/adafruit/Adafruit_FXAS21002C/blob/master/Adafruit_FXAS21002C.cpp
https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.h
https://github.com/adafruit/Adafruit_FXOS8700/blob/master/Adafruit_FXOS8700.cpp


/** Device ID for this sensor (used as a sanity check during init) */
                    static const int FXAS21002C_ID = 0xD7; // 1101 0111

                    
                    
                    /** Gyroscope sensitivity at 250dps */
                    #define GYRO_SENSITIVITY_250DPS (0.0078125F) // Table 35 of datasheet
                    /** Gyroscope sensitivity at 500dps */
                    #define GYRO_SENSITIVITY_500DPS (0.015625F)
                    /** Gyroscope sensitivity at 1000dps */
                    #define GYRO_SENSITIVITY_1000DPS (0.03125F)
                    /** Gyroscope sensitivity at 2000dps */
                    #define GYRO_SENSITIVITY_2000DPS (0.0625F)

/*
GYRO_RANGE_250DPS = 250,   /**< 250dps */
  //GYRO_RANGE_500DPS = 500,   /**< 500dps */
  //GYRO_RANGE_1000DPS = 1000, /**< 1000dps */
  //GYRO_RANGE_2000DPS = 2000  /**< 2000dps */
  

            };
        }
    }
}