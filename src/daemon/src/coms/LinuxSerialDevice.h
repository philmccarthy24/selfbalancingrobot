#pragma once
#include "ISerialDevice.h"
#include <string>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>

namespace sbrcontroller {
    namespace coms {

        struct SerialOperation {
            std::vector<unsigned char> data;
            int bytesToRead; // if 0, assumed to be a write with data supplied
        };

        class LinuxSerialDevice : public ISerialDevice
        {
        public:
            LinuxSerialDevice(const std::string& serialDeviceName, int baudRate = 115200);
            virtual ~LinuxSerialDevice();

            virtual std::future<std::vector<unsigned char>> ReadAsync(int numBytesToRead) const override;
            virtual std::future<void> WriteAsync(const std::vector<unsigned char>& bytesToWrite) const override;
        private:
            void SerialComsThreadProc(const std::string& serialDeviceName, int baudRate);

            std::thread m_tComsThread;
            std::atomic_bool m_bKillSignal;
            std::mutex m_comsMutex;
            std::queue<SerialOperation> m_ioQueue;
        };
    }
}