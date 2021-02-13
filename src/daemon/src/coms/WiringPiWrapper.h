#pragma once

#ifdef USING_WIRING_PI

#include "II2CDevice.h"

namespace sbrcontroller {
    namespace coms {

        class WiringPiWrapper : public II2CDevice {
        public:
            WiringPiWrapper(int deviceId);
            virtual ~WiringPiWrapper();

            virtual unsigned char ReadReg8(int i2creg) override;
            virtual void WriteReg8(int i2creg, unsigned char data) override;

            virtual unsigned short ReadReg16(int i2creg) override;
            virtual void WriteReg16(int i2creg, unsigned short data) override;

            virtual std::vector<unsigned char> ReadBlock(int i2creg) override;
            virtual void WriteBlock(int i2creg, const std::vector<unsigned char>& buffer) override;

        private:
            int m_fdI2CDevice;
        };
    }
}
#endif