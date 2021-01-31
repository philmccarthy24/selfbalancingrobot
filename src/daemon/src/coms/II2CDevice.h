#pragma once
#include <vector>

namespace sbrcontroller {
    namespace coms {

        class II2CDevice
        {
        public:
            II2CDevice(int deviceId) {}
            virtual ~II2CDevice() {}

            virtual unsigned char ReadReg8(int i2creg) = 0;
            virtual void WriteReg8(int i2creg, unsigned char data) = 0;

            virtual unsigned short ReadReg16(int i2creg) = 0;
            virtual void WriteReg16(int i2creg, unsigned short data) = 0;

            virtual std::vector<unsigned char> ReadBlock(int i2creg) = 0;
            virtual void WriteBlock(int i2creg, const std::vector<unsigned char>& buffer) = 0;
        };

    }
}