#include "MPU6050.h"
#include <cmath>
#include <stdexcept>
#include <chrono>
#include <wiringPiI2C.h>
#include <tuple>

using namespace std;

namespace sbrcontroller {
    namespace imu {

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
    m_imuFilter(50) // 50Hz (20ms pauses) is prob best we can hope to sample at for non-RTOS
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
    m_fDegreesPerSec = GYRO_FS_COUNTS[FS_SEL];

    //... there's a way to do a calibration as well
    // for both accel and gyro. Should we support this?

    // kick off the sampling thread
    m_tSampleUpdateThread = std::thread([this] {RawSampleUpdateThreadProc();});
}

MPU6050::~MPU6050()
{
    m_bKillSignal = true;
    m_tSampleUpdateThread.join();
}

unsigned short MPU6050::ReadHighLowI2CRegisters(int highRegister)
{
    int highByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister);
    int lowByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister + 1);
    return (highByte << 8) | lowByte;
}

void MPU6050::RawSampleUpdateThreadProc()
{
    while (!m_bKillSignal) {
        // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
        short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
        short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
        short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));
        short gyXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_XOUT_H));
        short gyYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_YOUT_H));
        short gyZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(GYRO_ZOUT_H));

        // convert to units
        float accX = static_cast<float>(accXRawCounts) / static_cast<float>(m_nCountsPerG);
        float accY = static_cast<float>(accYRawCounts) / static_cast<float>(m_nCountsPerG);
        float accZ = static_cast<float>(accZRawCounts) / static_cast<float>(m_nCountsPerG);
        float gyX = static_cast<float>(gyXRawCounts) / static_cast<float>(m_fDegreesPerSec);
        float gyY = static_cast<float>(gyYRawCounts) / static_cast<float>(m_fDegreesPerSec);
        float gyZ = static_cast<float>(gyZRawCounts) / static_cast<float>(m_fDegreesPerSec);

        m_imuFilter.Update6dofIMU(DegreesToRadians(gyX), DegreesToRadians(gyY), DegreesToRadians(gyZ), accX, accY, accZ);

        // we sample at 50Hz, so wait 20ms between readings. this won't be
        // massively accurate unfortunately (TODO: add metrics)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

double MPU6050::RadiansToDegrees(double radians)
{
    return radians * (180.0 / M_PI);
}

double MPU6050::DegreesToRadians(double degrees)
{
    return (degrees * M_PI) / 180;
}

Orientation3D MPU6050::ReadOrientation()
{
    auto qfuture = m_imuFilter.RequestSensorReading();
    auto q = qfuture.get(); // wait for the promise to be fulfilled

    // convert quarternion to orientation (euler angles)
    Orientation3D angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.q0 * q.q1 + q.q2 * q.q3);
    double cosr_cosp = 1 - 2 * (q.q1 * q.q1 + q.q2 * q.q2);
    angles.m_dbRoll_deg = RadiansToDegrees(std::atan2(sinr_cosp, cosr_cosp));

    // pitch (y-axis rotation)
    double sinp = 2 * (q.q0 * q.q2 - q.q3 * q.q1);
    if (std::abs(sinp) >= 1)
        angles.m_dbPitch_deg = RadiansToDegrees(std::copysign(M_PI / 2, sinp)); // use 90 degrees if out of range
    else
        angles.m_dbPitch_deg = RadiansToDegrees(std::asin(sinp));

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.q0 * q.q3 + q.q1 * q.q2);
    double cosy_cosp = 1 - 2 * (q.q2 * q.q2 + q.q3 * q.q3);
    angles.m_dbYaw_deg = RadiansToDegrees(std::atan2(siny_cosp, cosy_cosp));

    return angles;
}

}
}