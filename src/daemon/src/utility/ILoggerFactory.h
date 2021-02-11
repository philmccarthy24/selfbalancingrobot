#pragma once
#include <memory>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {
    namespace utility {

        class ILoggerFactory
        {
        public:
            ILoggerFactory() {}
            virtual ~ILoggerFactory() {}

            virtual std::shared_ptr<spdlog::logger> CreateLogger(const std::string& className) const = 0;
        };
    }
}