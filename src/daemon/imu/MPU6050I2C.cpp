#include "MPU6050I2C.h"
#include <cmath>
#include <stdexcept>
#include <chrono>
#include <wiringPiI2C.h>
#include <tuple>

using namespace std;

namespace sbrcontroller 
{
    namespace imu
    {

const int MPU6050I2C::ACCEL_FS_COUNTS[] = {
    16384, // range of accelerometer is +-2g
    8192, // range of accelerometer is +-4g
    4096, // range of accelerometer is +-8g
    2048  // range of accelerometer is +-16g
};

const double MPU6050I2C::GYRO_FS_COUNTS[] = {
    131, // range of gyroscope is +-250deg/s
    65.5, // range of gyroscope is +-500deg/s
    32.8, // range of gyroscope is +-1000deg/s
    16.4  // range of gyroscope is +-2000deg/s
};

MPU6050I2C::MPU6050I2C(bool bEnableRawReads)
{
    m_fdMPU6050 = wiringPiI2CSetup(MPU6050_addr);
    if (m_fdMPU6050 == -1) {
        throw runtime_error("Could not setup MPU6050");
    }

    if (bEnableRawReads) {
        // disable sleep mode by setting PWR_MGMT_1 register to 0
        wiringPiI2CWriteReg8(m_fdMPU6050, PWR_MGMT_1, 0x00);
    }
    
    //... there's a way to do a calibration as well
    // for both accel and gyro. Should we support this?
}

MPU6050I2C::~MPU6050I2C()
{
}

int MPU6050I2C::ReadAccelFS()
{
    return (wiringPiI2CReadReg8(m_fdMPU6050, ACCEL_CONFIG) >> 3) & 0x3;
}

int MPU6050I2C::ReadGyroFS()
{
    return (wiringPiI2CReadReg8(m_fdMPU6050, GYRO_CONFIG) >> 3) & 0x3;
}

unsigned short MPU6050I2C::ReadHighLowI2CRegisters(int highRegister)
{
    int highByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister);
    int lowByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister + 1);
    return (highByte << 8) | lowByte;
}

MPU6050I2C::RawCounts MPU6050I2C::ReadAccelerometerRaw()
{
    // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
    short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
    short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
    short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));

    return MPU6050I2C::RawCounts { accX, accY, accZ };
}

MPU6050I2C::RawCounts MPU6050I2C::ReadGyroRaw()
{
    // read raw counts from gyroscope. page 29 of [RegisterMap]
    short gyXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_XOUT_H));
    short gyYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_YOUT_H));
    short gyZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_ZOUT_H));

    return MPU6050I2C::RawCounts {gyX, gyY, gyZ };
}

}
}
