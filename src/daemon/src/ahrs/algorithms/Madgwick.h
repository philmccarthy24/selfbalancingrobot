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
#include "AHRS.h"

namespace sbrcontroller {
    namespace ahrs {
        namespace algorithms {
            
            class Madgwick : public IAHRSFusionAlgorithm
            {
            public:
                Madgwick(float sampleFreq, float betaDef = 0.1f);
                ~Madgwick();

                virtual bool IsHardwareImplementation() override;
                virtual void Update(const Axis3DSensorData& gyroData, const Axis3DSensorData& accelData, const Axis3DSensorData& magData) override;
                virtual void UpdateIMU(const Axis3DSensorData& gyroData, const Axis3DSensorData& accelData) override;

                virtual std::future<Quarternion> ReadFusedSensorDataAsync() override;

            private:
                float invSqrt(float x);

                volatile float beta;	// algorithm gain, default   0.1f	(2 * proportional gain);
                float m_fSampleFreqHz;
                volatile Quarternion m_q;	// quaternion of sensor frame relative to auxiliary frame
                Quarternion m_qOut;
                std::promise<Quarternion> m_readingPromise;
                volatile bool m_bSignalRequestReading;
            };
        }
    }
}

