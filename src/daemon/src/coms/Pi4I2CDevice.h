#pragma once

#include "II2CDevice.h"

namespace sbrcontroller {
    namespace coms {

        class Pi4I2CDevice : public II2CDevice {
        public:
            Pi4I2CDevice(int deviceId);
            virtual ~Pi4I2CDevice();

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