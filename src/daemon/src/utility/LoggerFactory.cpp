#include "LoggerFactory.h"
#include "Register.h"
#include "IConfigSection.h"
#include "sbrcontroller.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <memory>
#include <algorithm>

namespace sbrcontroller {
    namespace utility {

        class LoggerFactoryImpl : public ILoggerFactory
        {
        public:
            LoggerFactoryImpl()
            {
                // register async logger thread pool - do this once only
                spdlog::init_thread_pool(8192, 2);

                // create the logging sinks
                auto& config = Register::Config();
                auto sinksConfig = config.GetConfigSections(LOGGING_SINKS_KEY);

                if (sinksConfig.size() > 0) {
                    m_sinks.reserve(sinksConfig.size());
                    for (auto sinkConfig : sinksConfig) 
                    {
                        m_sinks.push_back(CreateSink(sinkConfig));
                    }
                }

                // get the global logging level and pattern from config
                m_globalLogLevel = spdlog::level::info;
                try {
                    // optional setting, ignore exceptions
                    auto level = config.GetConfigValue(LOGGING_LEVEL_KEY);
                    m_globalLogLevel = spdlog::level::from_str(level);
                } catch (const std::exception& e) {
                }
                m_globalLogPattern = "%+"; // default pattern
                try {
                    // optional setting, ignore exceptions
                    m_globalLogPattern = config.GetConfigValue(LOGGING_PATTERN_KEY);
                } catch (const std::exception& e) {
                }
            }

            virtual ~LoggerFactoryImpl()
            {
            }

            // sinks types one of {stdout_color_sink_mt, stderr_color_sink_mt, basic_file_sink_mt, syslog_logger_sink_mt, rotating_file_sink_mt, daily_logger_sink_mt}
            // loggers config let you override settings per logger instance
            virtual std::shared_ptr<spdlog::logger> CreateLogger(const std::string& className) const override
            {
                std::shared_ptr<spdlog::logger> newClassLogger = nullptr;
            
                auto classLoggerLevel = m_globalLogLevel;
                auto classLoggerPattern = m_globalLogPattern;

                if (m_sinks.size() > 0)
                {
                    // logging is enabled
                    // check config for matching specific class logger configuration (overrides)
                    auto& config = Register::Config();
                    auto loggersConfig = config.GetConfigSections(LOGGING_LOGGERS_KEY);
                    
                    auto matchingLoggerConfig = std::find_if(std::begin(loggersConfig), std::end(loggersConfig), [&] (const std::shared_ptr<IConfigSection>& cs) {
                        try {
                            return cs->GetConfigValue("name") == className;
                        } catch (const std::exception& e) {
                            return false;
                        }
                    });
                    if (matchingLoggerConfig != std::end(loggersConfig)) {
                        try {
                            auto configuredClassLoggerLevel = (*matchingLoggerConfig)->GetConfigValue("level");
                            //found
                            classLoggerLevel = spdlog::level::from_str(configuredClassLoggerLevel);
                        } catch (const std::exception& e) {
                            // if not present/configured, ignore
                        }
                        try {
                            classLoggerPattern = (*matchingLoggerConfig)->GetConfigValue("pattern");
                            
                        } catch (const std::exception& e) {
                            // if not present/configured, ignore
                        }
                    }

                    auto tp = spdlog::thread_pool();
                    newClassLogger = std::make_shared<spdlog::async_logger>(className, m_sinks.begin(), m_sinks.end(), tp, spdlog::async_overflow_policy::block);
                    spdlog::register_logger(newClassLogger);
                    newClassLogger->set_level(classLoggerLevel);
                    newClassLogger->set_pattern(classLoggerPattern);

                }
                return newClassLogger;
            }

        private:
            spdlog::sink_ptr CreateSink(const std::shared_ptr<IConfigSection>& sinkConfig)
            {
                auto sinkType = sinkConfig->GetConfigValue("type");
                spdlog::sink_ptr pSink = nullptr;
                if (sinkType == "stdout_color_sink_mt") {
                    pSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                }
                else if (sinkType == "rotating_file_sink_mt") {
                    auto maxSize = std::stoi(sinkConfig->GetConfigValue("maxSize"));
                    auto maxFiles = std::stoi(sinkConfig->GetConfigValue("maxFiles"));
                    auto logFileRoot = sinkConfig->GetConfigValue("logFileRoot");
                    pSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileRoot, maxSize, maxFiles);
                }
                /// else ...
                // ... more sinks here

                return pSink;
            }

            spdlog::level::level_enum m_globalLogLevel;
            std::string m_globalLogPattern;
            std::vector<spdlog::sink_ptr> m_sinks;
        };


        //////////////////////////////////////////////////////////////////
        ////// PIMPL forwarders
        //////////////////////////////////////////////////////////////////

        LoggerFactory::LoggerFactory()
        {
            p_impl = std::make_shared<LoggerFactoryImpl>();
        }

        LoggerFactory::~LoggerFactory()
        {
        }

        std::shared_ptr<spdlog::logger> LoggerFactory::CreateLogger(const std::string& className) const
        {
            return p_impl->CreateLogger(className);
        }

    }
}