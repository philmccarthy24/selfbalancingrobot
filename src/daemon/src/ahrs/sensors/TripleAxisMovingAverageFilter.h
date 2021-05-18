#pragma once
#include "AHRS.h"
#include "sensors.h"
#include <deque>
#include <memory>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {

    namespace ahrs {
        namespace sensors {

            
            class TripleAxisMovingAverageFilter : public sbrcontroller::sensors::ISensor 
            {
            public:
                TripleAxisMovingAverageFilter(int windowSize, std::shared_ptr<sbrcontroller::sensors::ISensor> pActualSensor, std::shared_ptr<spdlog::logger> pLogger);
                virtual ~TripleAxisMovingAverageFilter();

                virtual sbrcontroller::sensors::SensorInfo GetDeviceInfo() override;
                virtual int ReadSensorData(unsigned char* buffer, unsigned int length) override;
                virtual void ClearCalibration() override;

            private:
                sbrcontroller::sensors::TripleAxisData CalcAverage();
                //TripleAxisData CalcAverageB();

                std::shared_ptr<sbrcontroller::sensors::ISensor> m_pActualSensor;
                int m_nWindowSize;
                std::deque<sbrcontroller::sensors::TripleAxisData> m_sensorDataWindow;
                
                std::shared_ptr<spdlog::logger> m_pLogger;
                
                // can we instead record the average, and then (a*4 + new reading) / 5 => new average?
                // think this is the same
                //TripleAxisData m_currentAverage;
            };

        }
    }
}