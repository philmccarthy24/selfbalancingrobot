#include "StringReaderWriter.h"
#include "IDataDevice.h"
#include "sbrcontroller.h"
#include <fmt/core.h>
#include <chrono>

namespace sbrcontroller {
    namespace coms {

        // m_pReadHead is where the read buffer starts
        // m_pReadTail points to the end of the last buffered byte of data

        StringReaderWriter::StringReaderWriter(std::shared_ptr<IDataDevice> pDataDevice) :
            StringReaderWriter(pDataDevice, DEFAULT_READ_BUFFER_SZ, DEFAULT_IO_TIMEOUT_MS)
        {
        }

        StringReaderWriter::StringReaderWriter(std::shared_ptr<IDataDevice> pDataDevice, int readBufferSize, int ioTimeoutMS) :
            m_pDataDevice(pDataDevice),
            READ_BUFFER_SZ(readBufferSize),
            IO_TIMEOUT_MS(ioTimeoutMS)
        {
            m_readBuffer.resize(READ_BUFFER_SZ);
            m_pReadHead = m_pReadTail = &m_readBuffer[0];
        }

        StringReaderWriter::~StringReaderWriter()
        {
        }

        bool StringReaderWriter::IsUnderflowed()
        {
            return m_pReadHead == m_pReadTail;
        }

        void StringReaderWriter::HandleUnderflow()
        {
            
        }

        std::string StringReaderWriter::Read(char untilChar)
        {
            std::vector<char> readString;
            readString.reserve(1024);

            // we need to have some kind of timeout mechanism in here, otherwise
            // we could loop forever.
            auto startReadTP = std::chrono::system_clock::now();
            /*
            auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            auto epoch = now_ms.time_since_epoch();
            auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            long duration = value.count();
            */
            do {
                if (IsUnderflowed())
                {
                    // handle underflow
                    char* pEndBuf = &m_readBuffer[READ_BUFFER_SZ];
                    while (IsUnderflowed())
                    {
                        if (m_pReadTail == pEndBuf)
                        {
                            // exhausted buffer, reset to beginning
                            m_pReadHead = m_pReadTail = &m_readBuffer[0];
                        }
                        int bytesRead = m_pDataDevice->Read(m_pReadTail, pEndBuf - m_pReadTail);
                        m_pReadTail += bytesRead;
                        // check elapsed time
                        auto currentReadTP = std::chrono::system_clock::now();
                        auto msDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentReadTP-startReadTP);
                        if (msDuration.count() > IO_TIMEOUT_MS) {
                            throw errorhandling::ComsException(fmt::format("Timed out after {} ms attempting to read", msDuration.count()));
                        }
                    }
                }
                readString.push_back(*m_pReadHead);
            } while (*m_pReadHead++ != untilChar);

            return std::string(readString.begin(), readString.end());
        }

        void StringReaderWriter::Write(const std::string& asciiMessage)
        {
            std::vector<char> writeBuf(asciiMessage.begin(), asciiMessage.end());
            auto startWriteTP = std::chrono::system_clock::now();
            int numBytesToWrite = writeBuf.size();
            int numBytesLeftToWrite = numBytesToWrite;
            while (numBytesLeftToWrite > 0) {
                numBytesLeftToWrite -= m_pDataDevice->Write(&writeBuf[numBytesToWrite - numBytesLeftToWrite], numBytesLeftToWrite);
                // check elapsed time
                auto currentWriteTP = std::chrono::system_clock::now();
                auto msDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentWriteTP-startWriteTP);
                if (msDuration.count() > IO_TIMEOUT_MS) {
                    throw errorhandling::ComsException(fmt::format("Timed out after {} attempting to write", msDuration.count()));
                }
            }
            
        }
    }

}