#pragma once
#include "IDataDevice.h"
#include <map>

namespace sbrcontroller {
    namespace coms {

        class LinuxSerialDevice : public IDataDevice
        {
        public:
            LinuxSerialDevice(const std::string& serialDeviceName, int baudRate = 115200);
            virtual ~LinuxSerialDevice();

            virtual int Read(char* bufferToRead, int bufLen) const override;
            virtual int Write(char* bufferToWrite, int bufLen) const override;

        private:
            int m_fdSerialPort;
        };
    }
}