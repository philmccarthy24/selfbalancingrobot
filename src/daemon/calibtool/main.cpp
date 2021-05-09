#include "Register.h"
#include "FileConfigProvider.h"
#include "JSONConfig.h"
#include "SBRCommon.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "Calibration.h"
#include "AHRS.h"
#include "AHRSDump.h"
#include "LinuxSerialDevice.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include <iostream>
#include <unistd.h>
#include <memory>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <atomic>
#include <fmt/core.h>

using namespace std;
using namespace sbrcontroller;

/********************
 * 
 * Calibration tool and test harness for visualisation of device orientation
 * 
 * For calibration, ensure your sbrconfig.json is set up correctly with your device setings,
 * then run
 *  calibtool -calibrate
 * 
 * To use calibtool as a source of ahrs data for web visualisation, run
 *  calibtool -ahrsdump (-q)
 * use the -q flag to dump output in Quaternions format instead of Euler angles.
 *
 * The visualiser website that can read the output data is at:
 * https://adafruit.github.io/Adafruit_WebSerial_3DModelViewer/
 * You will need to enable the #enable-experimental-web-platform-features flag in chrome://flags
 * You will also need to enable the /dev/ttyS0 miniUART on GPIO pins 14 and 15 using
 * sudo raspi-config (configure enabled but not a linux boot terminal)
 * Then connect GPIO pins 14 and 15 with a cable to create a serial loopback
 * 
 ***********************/

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
    if (args.empty()) {
        printf("Calibtool: usage\ncalibtool -calibrate\n\tRuns a calibration of Gyro and Magnetometer\n");
        printf("calibtool -ahrsdump (-q) (-s)\n\tOutputs visualisation data (optionally to serial)\n");
        exit(0);
    }

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

            if (args[0] == "-calibrate")
            {
                // create sensors from config
                std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>> sensors;
                auto sensorConfig = utility::Register::Config().GetConfigSections(sbrcontroller::AHRS_SENSOR_IDS_KEY);
                for (auto& sensorConfig : sensorConfig)
                {
                    sensors.push_back(pFactory->CreateSensor(sensorConfig));
                }

                calibtool::Calibration cal(sensors);

                // first let's calibrate the gyro at rest.
                cal.PerformGyroCalibration();

                // next let's calibrate the accelerometer at rest.
                cal.PerformAccelerometerCalibration();

                // Now the magnetometer.
                cal.PerformMagnetometerCalibration();

                printf("Calibration complete. Final calibration data:\n");
                printf(fmt::format("Gyro Zero Rate Offset: x={}, y={}, z={}\n", cal.GetGyroCalibration().x, cal.GetGyroCalibration().y, cal.GetGyroCalibration().z).c_str());
                printf(fmt::format("Accel Zero Rate Offset: x={}, y={}, z={}\n", cal.GetAccelerometerCalibration().x, cal.GetAccelerometerCalibration().y, cal.GetAccelerometerCalibration().z).c_str());
                printf(fmt::format("Magnetometer Hard Iron Offset: x={}, y={}, z={}\n", cal.GetMagnetometerCalibration().x, cal.GetMagnetometerCalibration().y, cal.GetMagnetometerCalibration().z).c_str());
            }
            else if (args[0] == "-ahrsdump") {
                bool useQuaternions = args.size() > 1 && args[1] == "-q";
                bool useSerial = args.size() > 2 && args[2] == "-s";
                printf(fmt::format("Dumping ahrs data in {} format to {}.\n", useQuaternions ? "Quaternion" : "Euler angle", useSerial ? "/dev/ttyS0" : "console").c_str());
                printf("Press ENTER to continue. Press ENTER to then stop\n");
                getchar();

                auto ahrsDataSource = utility::Register::Factory().CreateAHRSDataSource();
                std::shared_ptr<sbrcontroller::coms::ISerialDevice> pSerialDev = nullptr;
                if (useSerial) 
                {
                    auto pSerialLogger = utility::Register::LoggerFactory().CreateLogger("SerialLogger");
                    pSerialDev = std::make_shared<sbrcontroller::coms::LinuxSerialDevice>(pSerialLogger, "/dev/ttyS0");
                }
                auto ahrsDump = std::make_shared<calibtool::AHRSDump>(pSerialDev, useQuaternions);
                ahrsDataSource->Register("AHRSDump", ahrsDump, 200);
                
                // the ahrs publish thread is now running and calling the ahrsDump handler
                getchar();

                ahrsDataSource->Unregister("AHRSDump");
            }
                
        }
        catch (const std::exception& ex) {
            logger->error(ex.what());
        }
    } catch (std::exception& ex) {
        // all we can do is use stderr
        cerr << ex.what() << std::endl;
    }
}


