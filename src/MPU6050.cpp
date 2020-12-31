#include "MPU6050.h"
#include <cmath>
#include <stdexcept>
#include <wiringPiI2C.h>

using namespace std;

namespace sbr {

const int MPU6050::ACCEL_FS_COUNTS[] = {
    16384, // range of accelerometer is +-2g
    8192, // range of accelerometer is +-4g
    4096, // range of accelerometer is +-8g
    2048  // range of accelerometer is +-16g
};

MPU6050::MPU6050()
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
    //int FS_SEL = (wiringPiI2CReadReg8(fd_mpu6050, 0x1B) >> 3) & 0x3;

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

pair<double, double> MPU6050::ReadOrientation()
{
    // read some raw counts. page 29 of [RegisterMap]
    short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
    short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
    short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));

    double accX = static_cast<double>(accXRawCounts) / static_cast<double>(m_nCountsPerG);
    double accY = static_cast<double>(accYRawCounts) / static_cast<double>(m_nCountsPerG);
    double accZ = static_cast<double>(accZRawCounts) / static_cast<double>(m_nCountsPerG);
    double xRot = -RadsToDegrees(atan2(accX, Dist(accY, accZ)));
    double yRot = RadsToDegrees(atan2(accY, Dist(accX, accZ)));

    /* don't appear to need gyroscope data
    short gyXRawCounts = ReadHighLowI2CRegisters(0x43);
    short gyYRawCounts = ReadHighLowI2CRegisters(0x45);
    short gyZRawCounts = ReadHighLowI2CRegisters(0x47);
    //cout << "AccelX=" << accX << ", AccelY=" << accY << ", AccelZ=" << accZ;
    //cout << "GyroX=" << gyX << ", GyroY=" << gyY << ", GyroZ=" << gyZ << endl;
    */

    return make_pair(xRot, yRot);
}

}