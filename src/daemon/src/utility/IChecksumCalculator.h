#pragma once
#include <string>

namespace sbrcontroller {
    namespace utility {

        class IChecksumCalculator
        {
        public:
            IChecksumCalculator() {}
            virtual ~IChecksumCalculator() {}

            virtual std::string Calculate(const std::string& source) const = 0;
        };
    }
}