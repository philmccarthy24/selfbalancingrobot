#pragma once
#include <vector>
#include <string>

namespace sbrcontroller {
    namespace coms {

        class IDataDevice
        {
        public:
            IDataDevice() {}
            virtual ~IDataDevice() {}

            virtual int Read(char* bufferToRead, int bufLen) const = 0;
            virtual int Write(char* bufferToWrite, int bufLen) const = 0;
        };
    }
}