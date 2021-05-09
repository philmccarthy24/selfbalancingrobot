#include "AHRSDump.h"
#include "ISerialDevice.h"
#include <fmt/core.h>

namespace calibtool {

    AHRSDump::AHRSDump(std::shared_ptr<sbrcontroller::coms::ISerialDevice> pSerialDevice, bool useQuaternions) :
    m_pSerialDevice(pSerialDevice),
    m_bUseQuaternions(useQuaternions)
    {
    }

    AHRSDump::~AHRSDump()
    {
    }

    void AHRSDump::OnUpdate(const sbrcontroller::ahrs::Quaternion& orientation)
    {
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