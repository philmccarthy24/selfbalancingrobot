#include "TripleAxisMovingAverageFilter.h"
#include "spdlog/spdlog.h"

using namespace sbrcontroller::sensors;

namespace sbrcontroller {

    namespace ahrs {
        namespace sensors {

            TripleAxisMovingAverageFilter::TripleAxisMovingAverageFilter(int windowSize, std::shared_ptr<sbrcontroller::sensors::ISensor> pActualSensor, std::shared_ptr<spdlog::logger> pLogger) :
            m_nWindowSize(windowSize),
            m_pActualSensor(pActualSensor),
            m_pLogger(pLogger)
            {
            }
            
            TripleAxisMovingAverageFilter::~TripleAxisMovingAverageFilter()
            {
            }

            sbrcontroller::sensors::SensorInfo TripleAxisMovingAverageFilter::GetDeviceInfo() 
            {
                return m_pActualSensor->GetDeviceInfo();
            }
                
            int TripleAxisMovingAverageFilter::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                m_pActualSensor->ReadSensorData(buffer, length);
                TripleAxisData* pData = reinterpret_cast<TripleAxisData*>(buffer);
                if (m_sensorDataWindow.size() >= m_nWindowSize)
                    m_sensorDataWindow.pop_front();
                m_sensorDataWindow.push_back(*pData);
                m_pLogger->trace("Stored latest sensor reading {}, {}, {}", pData->x, pData->y, pData->z);
                *pData = CalcAverage();
                m_pLogger->trace("Updated average = {}, {}, {}", pData->x, pData->y, pData->z);
                return sizeof(TripleAxisData);
            }
             
            void TripleAxisMovingAverageFilter::ClearCalibration()
            {
                m_pActualSensor->ClearCalibration();
            }

            TripleAxisData TripleAxisMovingAverageFilter::CalcAverage()
            {
                TripleAxisData av = {};
                for (auto& tad : m_sensorDataWindow) 
                {
                    av.x += tad.x;
                    av.y += tad.y;
                    av.z += tad.z;
                }
                av.x /= m_sensorDataWindow.size();
                av.y /= m_sensorDataWindow.size();
                av.z /= m_sensorDataWindow.size();
                return av;
            }

/*
            TripleAxisData TripleAxisMovingAverageFilter::CalcAverageB()
            {
                m_currentAverage
            }
            */
        }
    }
}