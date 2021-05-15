#include "AHRSDump.h"
#include "ISerialDevice.h"
#include <fmt/core.h>

namespace calibtool {

    AHRSDump::AHRSDump(std::shared_ptr<sbrcontroller::coms::ISerialDevice> pSerialDevice, bool useQuaternions) :
    m_pSerialDevice(pSerialDevice),
    m_bUseQuaternions(useQuaternions)
    {
        m_lastLoopTime = std::chrono::high_resolution_clock::now();
    }

    AHRSDump::~AHRSDump()
    {
    }

    void AHRSDump::OnUpdate(const sbrcontroller::ahrs::Quaternion& orientation)
    {
        // timing code to proove update handler is being called in a timely fashion
        auto updateTime = std::chrono::high_resolution_clock::now();
        auto totalElapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(updateTime - m_lastLoopTime);
        int count = totalElapsedUS.count();
        //printf("Update loop total time %d us\n", count);
        m_lastLoopTime = updateTime;

        std::string datum;
        if (!m_bUseQuaternions) {
            auto currOrientation = orientation.ToEuler();
            datum = fmt::format("Orientation: {:3.2f}, {:3.2f}, {:3.2f}\n", currOrientation.GetRollInDegrees(), currOrientation.GetPitchInDegrees(), currOrientation.GetYawInDegrees());
        } else {
            datum = fmt::format("Quaternion: {:3.2f}, {:3.2f}, {:3.2f}, {:3.2f}\n", orientation.w, orientation.x, orientation.y, orientation.z);
        }
        
        if (m_pSerialDevice != nullptr)
            m_pSerialDevice->Write((char*)datum.c_str(), datum.size());
        else
            printf(datum.c_str());
    }

}