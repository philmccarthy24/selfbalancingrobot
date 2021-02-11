#pragma once
#include "ILoggerFactory.h"
#include <memory>
#include <vector>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {
    namespace utility {

        class LoggerFactory : public ILoggerFactory
        {
        public:
            LoggerFactory();
            virtual ~LoggerFactory();

            virtual std::shared_ptr<spdlog::logger> CreateLogger(const std::string& className) const override;
            
        private:
            std::shared_ptr<ILoggerFactory> p_impl;
        };
    }
}