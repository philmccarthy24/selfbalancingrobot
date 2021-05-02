#pragma once
#include <memory>
#include <string>
#include <vector>

namespace sbrcontroller {
    namespace coms {

        class ISerialDevice;

        class IStringReaderWriter
        {
        public:
            IStringReaderWriter() {}
            virtual ~IStringReaderWriter() {}

            virtual std::string Read(char untilChar = '\n') = 0;
            virtual void Write(const std::string& asciiMessage) = 0;
        };
    }
}