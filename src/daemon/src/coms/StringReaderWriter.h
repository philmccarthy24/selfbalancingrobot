#pragma once
#include <memory>
#include <string>
#include <vector>

namespace sbrcontroller {
    namespace coms {

        class IDataDevice;

        class StringReaderWriter
        {
        public:
            StringReaderWriter(std::shared_ptr<IDataDevice> pDataDevice);
            StringReaderWriter(std::shared_ptr<IDataDevice> pDataDevice, int readBufferSize, int ioTimeoutMS);
            virtual ~StringReaderWriter();

            virtual std::string Read(char untilChar = '\n');
            virtual void Write(const std::string& asciiMessage);

        private:
            bool IsUnderflowed();
            void HandleUnderflow();

            std::shared_ptr<IDataDevice> m_pDataDevice;

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