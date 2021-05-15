#include "Register.h"
#include "MemoryConfigProvider.h"
#include "JSONConfig.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "AHRS.h"
#include "SBRCommon.h"
#include "LinuxSerialDevice.h"
#include "StringReaderWriter.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <chrono>

using namespace std;
using namespace sbrcontroller;

namespace sbrcontroller {
    namespace test {

class MotorComsIntegrationTests : public ::testing::Test
{
protected:
    
    const std::string testConfig = R"CONFIG(
{
    "ahrs": {
        "fusionAlgorithm": "Madgwick",
        "sensorSampleRateHz": 20,
        "sensors": [
            "MPU6050_gyro",
            "MPU6050_accel"
        ]
    },
    "motorControl": {
        "coms": {
            "type": "serial",
            "serialPort": "/dev/ttyACM0",
            "baud": 115200
        },
        "motors": [
            {
                "name": "right",
                "id": 0,
                "reversed": false
            },
            {
                "name": "left",
                "id": 1,
                "reversed": true
            }
        ]
    },
    "logging": {
        "level": "info",
        "pattern": "%+",
        "sinks": [
            {
                "type": "stdout_color_sink_mt"
            }
        ],
        "loggers": []
    }
})CONFIG";

    virtual void SetUp()
    {      
     
    }

    virtual void TearDown()
    { 
    }
};


// TODO: mock serial port so we're not driving the actual odrive
TEST_F(MotorComsIntegrationTests, SimpleMotorDrivingTest)
{
    utility::Register::RegisterConfigService(
            make_shared<utility::JSONConfig>(
                make_shared<MemoryConfigProvider>(testConfig)
        ));

    auto pLoggerFactory = make_shared<utility::LoggerFactory>();
    utility::Register::RegisterLoggerFactory(pLoggerFactory);
    auto logger = pLoggerFactory->CreateLogger("SimpleMotorDrivingTest");

    try {
        
        auto pFactory = make_shared<utility::SBRProdFactory>();
        utility::Register::RegisterLoggerFactory(pLoggerFactory);
        utility::Register::RegisterFactory(pFactory);

        auto configSections = utility::Register::Config().GetConfigSections(MOTOR_CONTROL_COMS_KEY);
        if (configSections.size() != 1)
            throw errorhandling::ConfigurationException("Expecting a single motor control coms section");
        auto configSection = configSections[0];
        if (configSection->GetConfigValue("type") != "serial") 
            throw errorhandling::ConfigurationException("Expecting serial port setup info in motor control coms section");

        auto serialPort = configSection->GetConfigValue("serialPort");
        int baudRate = std::stoi(configSection->GetConfigValue("baud"));
        
        auto pRawSerial = make_shared<coms::LinuxSerialDevice>(pLoggerFactory->CreateLogger("LinuxSerialDevice"), serialPort, baudRate);
        auto pSerialReaderWriter = make_shared<coms::StringReaderWriter>(pRawSerial);
        std::string m0_cmd_SetClosedLoop = "w axis0.requested_state 8"; // AXIS_STATE_CLOSED_LOOP_CONTROL
        std::string m0_cmd_SetIdleState = "w axis0.requested_state 1"; // AXIS_STATE_IDLE
        std::string m0_cmd_SetVel1rpm = "v 0 1 0";
        std::string m0_cmd_SetVel0rpm = "v 0 0 0";

        logger->info("Sending: {}\n", m0_cmd_SetClosedLoop);
        pSerialReaderWriter->Write(m0_cmd_SetClosedLoop + "\n");
        
        logger->info("Sending: {}\n", m0_cmd_SetVel1rpm);
        pSerialReaderWriter->Write(m0_cmd_SetVel1rpm + "\n");

        // pause
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        // end pause
        logger->info("Sending: {}\n", m0_cmd_SetVel0rpm);
        pSerialReaderWriter->Write(m0_cmd_SetVel0rpm + "\n");
        
        logger->info("Sending: {}\n", m0_cmd_SetIdleState);
        pSerialReaderWriter->Write(m0_cmd_SetIdleState + "\n");

    }
    catch (const std::exception& ex) {
        logger->error(ex.what());
    }

    ASSERT_TRUE(true);

}

    }
}