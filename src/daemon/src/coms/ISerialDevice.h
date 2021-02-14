#pragma once
#include <future>
#include <vector>

namespace sbrcontroller {
    namespace coms {

        class ISerialDevice
        {
        public:
            ISerialDevice() {}
            virtual ~ISerialDevice() {}

            virtual std::future<std::vector<unsigned char>> ReadAsync(int numBytesToRead) const = 0;
            virtual std::future<void> WriteAsync(const std::vector<unsigned char>& bytesToWrite) const = 0;
        };
    }
}