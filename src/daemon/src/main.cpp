#include "Register.h"
#include "FileConfigProvider.h"
#include "JSONConfig.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include <iostream>
#include <memory>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>

//## TODO DEBUGGING, Remove
#include "sbrcontroller.h"
#include "LinuxSerialDevice.h"
#include "StringReaderWriter.h"
#include "IChecksumCalculator.h"

using namespace std;
using namespace sbrcontroller;

int main()
{
    try {
        utility::Register::RegisterConfigService(
                make_shared<utility::JSONConfig>(
                    make_shared<utility::FileConfigProvider>("./sbrconfig.json")
            ));
        auto pLoggerFactory = make_shared<utility::LoggerFactory>();
        auto logger = pLoggerFactory->CreateLogger("RootLogger");

        try {
            // production startup process
            
            auto pFactory = make_shared<utility::SBRProdFactory>();
            utility::Register::RegisterLoggerFactory(pLoggerFactory);
            utility::Register::RegisterFactory(pFactory);
            
            logger->info("SBRController running!");

            // more setup code here... TODO

            auto configSections = utility::Register::Config().GetConfigSections(MOTOR_CONTROL_COMS_KEY);
            if (configSections.size() != 1)
                throw errorhandling::ConfigurationException("Expecting a single motor control coms section");
            auto configSection = configSections[0];
            if (configSection->GetConfigValue("type") == "serial") {
                auto serialPort = configSection->GetConfigValue("serialPort");
                int baudRate = std::stoi(configSection->GetConfigValue("baud"));
                
                auto pRawSerial = make_shared<coms::LinuxSerialDevice>(pLoggerFactory->CreateLogger("LinuxSerialDevice"), serialPort, baudRate);
                auto pSerialReaderWriter = make_shared<coms::StringReaderWriter>(pRawSerial);
                auto pChecksumCalc = pFactory->CreateChecksumCalculator();
                std::string m0_cmd_SetClosedLoop = "w axis0.requested_state 8"; // AXIS_STATE_CLOSED_LOOP_CONTROL
                auto m0_cmd_SetClosedLoop_cs = pChecksumCalc->Calculate(m0_cmd_SetClosedLoop);
                std::string m0_cmd_SetIdleState = "w axis0.requested_state 1"; // AXIS_STATE_IDLE
                auto m0_cmd_SetIdleState_cs = pChecksumCalc->Calculate(m0_cmd_SetIdleState);
                std::string m0_cmd_SetVel1rpm = "v 0 1 0";
                auto m0_cmd_SetVel1rpm_cs = pChecksumCalc->Calculate(m0_cmd_SetVel1rpm);
                std::string m0_cmd_SetVel0rpm = "v 0 0 0";
                auto m0_cmd_SetVel0rpm_cs = pChecksumCalc->Calculate(m0_cmd_SetVel0rpm);

                logger->info("Sending: {}{}\n", m0_cmd_SetClosedLoop, m0_cmd_SetClosedLoop_cs);
                pSerialReaderWriter->Write(m0_cmd_SetClosedLoop + m0_cmd_SetClosedLoop_cs + "\n");
                
                logger->info("Sending: {}{}\n", m0_cmd_SetVel1rpm, m0_cmd_SetVel1rpm_cs);
                pSerialReaderWriter->Write(m0_cmd_SetVel1rpm + m0_cmd_SetVel1rpm_cs + "\n");

                // pause
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                // end pause
                logger->info("Sending: {}{}\n", m0_cmd_SetVel0rpm, m0_cmd_SetVel0rpm_cs);
                pSerialReaderWriter->Write(m0_cmd_SetVel0rpm + m0_cmd_SetVel0rpm_cs + "\n");
                
                logger->info("Sending: {}{}\n", m0_cmd_SetIdleState, m0_cmd_SetIdleState_cs);
                pSerialReaderWriter->Write(m0_cmd_SetIdleState + m0_cmd_SetIdleState_cs + "\n");
                
            }

            printf("Press Return to quit\n");  
            getchar();
            
            
        }
        catch (const std::exception& ex) {
            logger->error(ex.what());
        }
    } catch (std::exception& ex) {
        // all we can do is use stderr
        cerr << ex.what() << std::endl;
    }
}

//https://code.visualstudio.com/docs/languages/cpp

//http://wiringpi.com/reference/i2c-library/

//https://sudonull.com/post/97828-Ruby-and-C-Part-4-Friends-of-the-accelerometer-gyroscope-and-range-finder-with-Raphaeljs


//https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing ??

