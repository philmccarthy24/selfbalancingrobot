#include "GCodeChecksumCalc.h"
#include <fmt/core.h>
#include <string>

namespace sbrcontroller {
    namespace utility {

        GCodeChecksumCalc::GCodeChecksumCalc() 
        {
        }

        GCodeChecksumCalc::~GCodeChecksumCalc() 
        {
        }

        std::string GCodeChecksumCalc::Calculate(const std::string& source) const
        {
            char cs8bit = 0;
            for (const char& c : source)
            {
                cs8bit ^= c;
            }
            return fmt::format("*{:d}", cs8bit);
        }
    }
}