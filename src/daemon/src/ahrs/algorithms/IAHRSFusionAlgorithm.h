#pragma once

//#include "AHRS.h"
#include <string>
#include <future>

namespace sbrcontroller
{
    namespace sensors {
        struct TripleAxisData;
    }
    namespace ahrs
    {
        struct Quarternion;

        namespace algorithms {

            class IAHRSFusionAlgorithm
            {
            public:
                IAHRSFusionAlgorithm() {}
                virtual ~IAHRSFusionAlgorithm() {}
                virtual bool IsHardwareImplementation() = 0;
                virtual void Update(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData, const sensors::TripleAxisData& magData) = 0;
                virtual void UpdateIMU(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData) = 0;

                virtual std::future<Quarternion> ReadFusedSensorDataAsync() = 0;
            };

        }
    }
}