//=====================================================================================================
// MahonyAHRS.h
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 13/02/2021   Phil McCarthy   Ported to C++
//
//=====================================================================================================
#pragma once
#include "IAHRSFusionAlgorithm.h"
#include "AHRS.h"

namespace sbrcontroller {
    namespace ahrs {
        namespace algorithms {
            
            class Mahony : public IAHRSFusionAlgorithm
            {
            public:
                Mahony() {}
                virtual ~Mahony() {}
                virtual bool IsHardwareImplementation() override;
                virtual void Update(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData, const sensors::TripleAxisData& magData) override;
                virtual void UpdateIMU(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData) override;

                virtual std::future<Quarternion> ReadFusedSensorDataAsync() override;

            private:
                volatile float twoKp;			// 2 * proportional gain (Kp)
                volatile float twoKi;			// 2 * integral gain (Ki)
                float m_fSampleFreqHz;
                volatile Quarternion m_q;	// quaternion of sensor frame relative to auxiliary frame
                Quarternion m_qOut;
                std::promise<Quarternion> m_readingPromise;
                volatile bool m_bSignalRequestReading;
            };
        }
    }
}
