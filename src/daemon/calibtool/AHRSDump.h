#pragma once

#include "AHRS.h"
#include <memory>
#include <vector>

namespace sbrcontroller {
    namespace coms {
        class ISerialDevice;
    }
}

namespace calibtool {

    class AHRSDump : public sbrcontroller::ahrs::IAHRSDataSubscriber
    {
    public:
        AHRSDump(std::shared_ptr<sbrcontroller::coms::ISerialDevice> pSerialDevice, bool useQuaternions);
        virtual ~AHRSDump();

        virtual void OnUpdate(const sbrcontroller::ahrs::Quaternion& orientation) override;

    private:
        bool m_bUseSerial;
        bool m_bUseQuaternions;
        std::shared_ptr<sbrcontroller::coms::ISerialDevice> m_pSerialDevice;
    };

}