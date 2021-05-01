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
// Madgwick filter has superior accuracy and less computational overhead than a Kalman filter
// when used with 9dof sensor
//
//=====================================================================================================
#pragma once
#include "IAHRSFusionAlgorithm.h"
#include "AHRS.h"
#include <atomic>

namespace sbrcontroller {
    namespace ahrs {
        namespace algorithms {
            
            class Madgwick : public IAHRSFusionAlgorithm
            {
            public:
                Madgwick(float sampleFreq, float betaDef = 0.75f); // 0.1 was default // I tried 0.05 but this wasn't much better. nor was 0.75
                ~Madgwick();

                virtual bool IsHardwareImplementation() override;
                virtual void Update(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData, const sensors::TripleAxisData& magData) override;
                virtual void UpdateIMU(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData) override;

                virtual std::future<Quaternion> ReadFusedSensorDataAsync() override;

            private:
                float invSqrt(float x);

                volatile float beta;	// algorithm gain, default   0.1f	(2 * proportional gain);
                float m_fSampleFreqHz;
                volatile Quaternion m_q;	// quaternion of sensor frame relative to auxiliary frame
                Quaternion m_qOut;
                std::promise<Quaternion> m_readingPromise;
                std::atomic_bool m_bSignalRequestReading;
            };
        }
    }
}

