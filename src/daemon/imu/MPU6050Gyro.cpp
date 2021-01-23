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

const double MPU6050Gyro::GYRO_FS_COUNTS[] = {
    131, // range of gyroscope is +-250deg/s
    65.5, // range of gyroscope is +-500deg/s
    32.8, // range of gyroscope is +-1000deg/s
    16.4  // range of gyroscope is +-2000deg/s
};

MPU6050Gyro::MPU6050Gyro(std::shared<MPU6050I2C> sensorComs, std::weak_ptr<ISpacialSensor> initialReferenceSensor): 
    m_lnMillisSinceOrientationLastRead(0),
    m_pSensorComs(sensorComs)
{
    std::move(m_pInitialReferenceSensor(initialReferenceSensor));
    m_dbDegreesPerSec = GYRO_FS_COUNTS[m_pSensorComs->ReadGyroFS()];

    //... there's a way to do a calibration as well
    // for both accel and gyro. Should we support this?
}

MPU6050Gyro::~MPU6050Gyro()
{
}

Orientation3D MPU6050Gyro::ReadOrientation() 
{
    /*
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
    */

    // convert to meaningful units
    double gyX = static_cast<double>(gyXRawCounts) / static_cast<double>(m_dbDegreesPerSec);
    double gyY = static_cast<double>(gyYRawCounts) / static_cast<double>(m_dbDegreesPerSec);
    double gyZ = static_cast<double>(gyZRawCounts) / static_cast<double>(m_dbDegreesPerSec);
}


    }
}