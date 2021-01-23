//=====================================================================================================
// Madgwick.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
// 23/01/2021   Phil McCarthy   Ported to C++
//
//=====================================================================================================
#pragma once
#include <future>

struct Quarternion
{
    float q0;
    float q1;
    float q2;
    float q3;
};

class Madgwick
{
public:
    Madgwick(float sampleFreq, float betaDef = 0.1f);
    ~Madgwick();

    // for sensor data with magnetometer (AHRS)
    void Update9dofAHRS(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
    
    // for 6dof IMU sensors
    // gx, gy and gz in radians per second
    // ax, ay and az in any unit
    void Update6dofIMU(float gx, float gy, float gz, float ax, float ay, float az);

    // !this will deadlock if Update6dofIMU is called on same thread as caller
    std::future<Quarternion> RequestSensorReading();

private:
    float invSqrt(float x);

    volatile float beta;	// algorithm gain, default   0.1f	(2 * proportional gain);
    float m_fSampleFreqHz;
    volatile Quarternion m_q;	// quaternion of sensor frame relative to auxiliary frame
    Quarternion m_qOut;
    std::promise<Quarternion> m_readingPromise;
    volatile bool m_bSignalRequestReading;
};