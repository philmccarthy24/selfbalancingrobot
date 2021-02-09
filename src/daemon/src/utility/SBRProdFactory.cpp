#include "SBRProdFactory.h"
#include "AHRSController.h"
#include "Register.h"
#include "IConfigSection.h"
#include "sbrcontroller.h"
#include "sensors.h"
#include "Madgwick.h"
#include "MPU6050Accel.h"
#include "MPU6050Gyro.h"
#include "Pi4I2CDevice.h"
#include "WiringPiWrapper.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <memory>
#include <functional>

using namespace sbrcontroller::sensors;
using namespace sbrcontroller::coms;
using namespace sbrcontroller::ahrs;
using namespace sbrcontroller::ahrs::sensors;
using namespace std;

namespace sbrcontroller {
    namespace utility {

        SBRProdFactory::SBRProdFactory()
        {
        }

        SBRProdFactory::~SBRProdFactory()
        {
        }
        
        std::shared_ptr<II2CDevice> SBRProdFactory::CreateI2CDevice(int deviceId) const
        {
            return std::make_shared<WiringPiWrapper>(deviceId);
        }

        std::shared_ptr<ahrs::IAHRSDataSource> SBRProdFactory::CreateAHRSDataSource() const
        {
            auto fusionAlgorithm = CreateFusionAlgorithm();
            
            std::vector<std::shared_ptr<ISensor>> sensors;
            auto sensorIds = Register::Config().GetConfigListValue(AHRS_SENSOR_IDS_KEY);
            for (auto sensorId : sensorIds)
            {
                sensors.push_back(CreateSensor(sensorId));
            }

            int sensorSamplePeriodHz = std::stoi(Register::Config().GetConfigValue(AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY));
            return std::make_shared<ahrs::AHRSController>(fusionAlgorithm, sensors, sensorSamplePeriodHz);
        }

        std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> SBRProdFactory::CreateFusionAlgorithm() const
        {
            std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> pFusionAlgorithm = nullptr;
            int sensorSamplePeriodHz = std::stoi(Register::Config().GetConfigValue(AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY));

            auto selectedFusionAlgorithm = Register::Config().GetConfigValue(AHRS_FUSION_ALGORITHM_CONFIG_KEY);

            if (selectedFusionAlgorithm == "Madgwick")
            {
                pFusionAlgorithm = make_shared<ahrs::algorithms::Madgwick>(sensorSamplePeriodHz);
            }
            else
            {
                throw errorhandling::NotImplementedException("Code does not yet exist to create algorithm " + selectedFusionAlgorithm + " specified.");
            }
            return pFusionAlgorithm;
        }

        std::shared_ptr<ISensor> SBRProdFactory::CreateSensor(const std::string& config) const
        {
            std::shared_ptr<ISensor> pSensor = nullptr;
            if (config == "MPU6050_gyro") {
                auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                pSensor = std::make_shared<MPU6050Gyro>(pI2CDevice);
            } else if (config == "MPU6050_accel") {
                auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                pSensor = std::make_shared<MPU6050Accel>(pI2CDevice);
            } else {
                throw errorhandling::NotImplementedException("Code does not yet exist to create sensor " + config + " specified.");
            }
            return pSensor;
        }

        // sinks types one of {stdout_color_sink_mt, stderr_color_sink_mt, basic_file_sink_mt, syslog_logger_sink_mt, rotating_file_sink_mt, daily_logger_sink_mt}
        // loggers config let you override settings per logger instance
        std::shared_ptr<spdlog::logger> SBRProdFactory::CreateLogger(const std::string& className) const
        {
            // need to define a sink factory method lambda here, and call from both single and multi cases.
            auto sinkFactory = [&] (std::shared_ptr<IConfigSection> sinkConfig) -> spdlog::sink_ptr
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
                try {
                    // optional settings, ignore exceptions
                    auto level = sinkConfig->GetConfigValue("level");
                    pSink->set_level(spdlog::level::from_str(level));
                    auto pattern = sinkConfig->GetConfigValue("pattern");
                    pSink->set_pattern(pattern);
                }
                catch (const std::exception& e) 
                {
                }
                return pSink;
            };

            std::shared_ptr<spdlog::logger> configuredLoggers = nullptr;
            auto& config = Register::Config();
            auto sinksConfig = config.GetConfigSections(LOGGING_SINKS_KEY);
            if (sinksConfig.size() > 0) {
                // logging is enabled
                std::vector<spdlog::sink_ptr> sinks;
                for (auto sinkConfig : sinksConfig) {
                    sinks.push_back(sinkFactory(sinkConfig));
                }
                // filter sinks here based on max log level of requested logger in config (if there is one)
                // TODO...
                spdlog::init_thread_pool(8192, 1);
                configuredLoggers = std::make_shared<spdlog::async_logger>(className, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
                spdlog::register_logger(configuredLoggers);
            }

            return configuredLoggers;
        }
    }
}