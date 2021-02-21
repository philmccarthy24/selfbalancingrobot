#pragma once
#include <memory>
#include "IDataDevice.h"

namespace spdlog {
    class logger;
}

namespace sbrcontroller {
    namespace coms {

        class LinuxSerialDevice : public IDataDevice
        {
        public:
            LinuxSerialDevice(std::shared_ptr<spdlog::logger> pLogger, const std::string& serialDeviceName, int baudRate = 115200);
            virtual ~LinuxSerialDevice();

            virtual int Read(char* bufferToRead, int bufLen) const override;
            virtual int Write(char* bufferToWrite, int bufLen) const override;

        private:
            int m_fdSerialPort;

            std::shared_ptr<spdlog::logger> m_pLogger;
        };
    }
}