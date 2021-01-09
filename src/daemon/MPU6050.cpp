#include "MPU6050.h"
#include <cmath>
#include <stdexcept>
#include <chrono>
#include <wiringPiI2C.h>

using namespace std;

namespace sbr {

const int MPU6050::ACCEL_FS_COUNTS[] = {
    16384, // range of accelerometer is +-2g
    8192, // range of accelerometer is +-4g
    4096, // range of accelerometer is +-8g
    2048  // range of accelerometer is +-16g
};

const double MPU6050::GYRO_FS_COUNTS[] = {
    131, // range of gyroscope is +-250deg/s
    65.5, // range of gyroscope is +-500deg/s
    32.8, // range of gyroscope is +-1000deg/s
    16.4  // range of gyroscope is +-2000deg/s
};

MPU6050::MPU6050() : 
    m_lnMillisSinceOrientationLastRead(0)
{
    m_fdMPU6050 = wiringPiI2CSetup(MPU6050_addr);
    if (m_fdMPU6050 == -1) {
        throw runtime_error("Could not setup MPU6050");
    }
    // disable sleep mode by setting PWR_MGMT_1 register to 0
    wiringPiI2CWriteReg8(m_fdMPU6050, PWR_MGMT_1, 0x00);

    // ACCEL_CONFIG register. AFS_SEL is bits 3 and 4
    int ACCEL_FS = (wiringPiI2CReadReg8(m_fdMPU6050, ACCEL_CONFIG) >> 3) & 0x3;
    m_nCountsPerG = ACCEL_FS_COUNTS[ACCEL_FS];
    
    // GYRO_CONFIG register. FS_SEL is bits 3 and 4
    int FS_SEL = (wiringPiI2CReadReg8(m_fdMPU6050, GYRO_CONFIG) >> 3) & 0x3;
    m_dbDegreesPerSec = GYRO_FS_COUNTS[FS_SEL];

    //... there's a way to do a calibration as well
    // for both accel and gyro. Should we support this?
}

MPU6050::~MPU6050()
{
}

unsigned short MPU6050::ReadHighLowI2CRegisters(int highRegister)
{
    int highByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister);
    int lowByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister + 1);
    return (highByte << 8) | lowByte;
}

double MPU6050::Dist(double a, double b) {
    return sqrt((a * a) + (b * b));
}

double MPU6050::RadsToDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

Orientation3D MPU6050::ReadOrientation()
{
    // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
    short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
    short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
    short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));
    short gyXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_XOUT_H));
    short gyYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_YOUT_H));
    short gyZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_ZOUT_H));

    // convert to meaningful units
    double accX = static_cast<double>(accXRawCounts) / static_cast<double>(m_nCountsPerG);
    double accY = static_cast<double>(accYRawCounts) / static_cast<double>(m_nCountsPerG);
    double accZ = static_cast<double>(accZRawCounts) / static_cast<double>(m_nCountsPerG);
    double gyX = static_cast<double>(gyXRawCounts) / static_cast<double>(m_dbDegreesPerSec);
    double gyY = static_cast<double>(gyYRawCounts) / static_cast<double>(m_dbDegreesPerSec);
    double gyZ = static_cast<double>(gyZRawCounts) / static_cast<double>(m_dbDegreesPerSec);

    // calculate the roll and pitch from the accelerometer. this will not be
    // very accurate short term due to spikes from external forces, but is reasonably
    // accurate over time
    Orientation3D oriResult;
    oriResult.m_dbRoll_deg = -RadsToDegrees(atan2(accX, Dist(accY, accZ)));
    oriResult.m_dbPitch_deg = RadsToDegrees(atan2(accY, Dist(accX, accZ)));

    // integrate gyro readings to get current angle. this will be fairly accurate short
    // term but will suffer from drift long term
    auto duration = chrono::system_clock::now().time_since_epoch();
    auto millis = chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    if (m_tsOrientationLastRead == 0) {
        // first time we have been called - 
    }

    

    //cout << "AccelX=" << accX << ", AccelY=" << accY << ", AccelZ=" << accZ;
    //cout << "GyroX=" << gyX << ", GyroY=" << gyY << ", GyroZ=" << gyZ << endl;
    

    return make_pair(xRot, yRot);
}

}

// can we reverse enginer
// https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6/MPU6050_DMP6.ino

// actually there is already a pi version
// https://github.com/jrowberg/i2cdevlib/tree/master/RaspberryPi_bcm2835/MPU6050
// take a look, maybe use this. maybe don't need to filter if using dmp?