#include "MPU6050I2C.h"
#include <cmath>

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

MPU6050Accel::MPU6050Accel(std::shared<MPU6050I2C> pSensorComs) :
    m_pSensorComs(pSensorComs)
{
    m_nCountsPerG = ACCEL_FS_COUNTS[m_pSensorComs->ReadAccelFS()];
    
    
    

    //... there's a way to do a calibration as well
    // for both accel and gyro. Should we support this?
}

MPU6050Accel::~MPU6050Accel()
{
}

double MPU6050Accel::Dist(double a, double b) 
{
    return sqrt((a * a) + (b * b));
}

double MPU6050Accel::RadsToDegrees(double radians) 
{
    return radians * (180.0 / M_PI);
}

Orientation3D MPU6050Accel::ReadOrientation() 
{
    // convert to meaningful units
    double accX = static_cast<double>(accXRawCounts) / static_cast<double>(m_nCountsPerG);
    double accY = static_cast<double>(accYRawCounts) / static_cast<double>(m_nCountsPerG);
    double accZ = static_cast<double>(accZRawCounts) / static_cast<double>(m_nCountsPerG);
    
    // calculate the roll and pitch from the accelerometer. this will not be
    // very accurate short term due to spikes from external forces, but is reasonably
    // accurate over time
    Orientation3D oriResult;
    oriResult.m_dbRoll_deg = -RadsToDegrees(atan2(accX, Dist(accY, accZ)));
    oriResult.m_dbPitch_deg = RadsToDegrees(atan2(accY, Dist(accX, accZ)));
}

}
}