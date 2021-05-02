#pragma once
#include "IStringReaderWriter.h"
#include <memory>
#include <string>
#include <vector>

namespace sbrcontroller {
    namespace coms {

        class ISerialDevice;

        class StringReaderWriter : public IStringReaderWriter
        {
        public:
            StringReaderWriter(std::shared_ptr<ISerialDevice> pSerialDevice);
            StringReaderWriter(std::shared_ptr<ISerialDevice> pSerialDevice, int readBufferSize, int ioTimeoutMS);
            virtual ~StringReaderWriter();

            virtual std::string Read(char untilChar = '\n') override;
            virtual void Write(const std::string& asciiMessage) override;

        private:
            bool IsUnderflowed();
            void HandleUnderflow();

            std::shared_ptr<ISerialDevice> m_pSerialDevice;

            std::vector<char> m_readBuffer;
            static const int DEFAULT_READ_BUFFER_SZ = 8192;
            static const int DEFAULT_IO_TIMEOUT_MS = 5000;
            const int READ_BUFFER_SZ;
            const int IO_TIMEOUT_MS;
            char* m_pReadHead;
            char* m_pReadTail;
        };
    }
}