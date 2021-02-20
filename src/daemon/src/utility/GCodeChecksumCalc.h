#pragma once
#include "IChecksumCalculator.h"
#include <string>

namespace sbrcontroller {
    namespace utility {

        class GCodeChecksumCalc : public IChecksumCalculator
        {
        public:
            GCodeChecksumCalc();
            virtual ~GCodeChecksumCalc();

            virtual std::string Calculate(const std::string& source) const override;
        };
    }
}