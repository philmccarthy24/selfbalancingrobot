#pragma once

#include "II2CDevice.h"

namespace sbrcontroller {
    namespace coms {

        class Pi4I2CDevice {
        public:
            Pi4I2CDevice(int deviceId);
            virtual ~Pi4I2CDevice();

            virtual unsigned char ReadReg8(int register) override;
            virtual void WriteReg8(int register, unsigned char data) override;

            virtual unsigned short ReadReg16(int register) override;
            virtual void WriteReg16(int register, unsigned short data) override;

            virtual std::vector<unsigned char> ReadBlock(int register) override;
            virtual void WriteBlock(int register, const std::vector<unsigned char>& buffer) override;

        private:
            int m_fdI2CDevice;
        }
    }
}