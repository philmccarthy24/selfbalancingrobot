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




Orientation3D MPU6050::ReadOrientation()
{
    auto qfuture = m_imuFilter.RequestSensorReading();
    auto q = qfuture.get(); // wait for the promise to be fulfilled

    
}

}
}