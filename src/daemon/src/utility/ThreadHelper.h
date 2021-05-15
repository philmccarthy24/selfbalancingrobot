#pragma once

#include <memory>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {
    namespace utility {

        class ThreadHelper
        {
        public:
            ThreadHelper();
            virtual ~ThreadHelper();
            static void SetRealtimePriority(std::shared_ptr<spdlog::logger> pLogger);
        };
    }
}
        