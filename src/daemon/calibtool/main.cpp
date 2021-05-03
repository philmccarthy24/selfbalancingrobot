#include "Register.h"
#include "FileConfigProvider.h"
#include "JSONConfig.h"
#include "SBRCommon.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "Calibration.h"
#include "AHRS.h"
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
        printf("calibtool -ahrsdump (-q)\n\tOutputs visualisation data\n");
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
                printf(fmt::format("Dumping ahrs data in {} format to /dev/ttyS0.\n", useQuaternions ? "Quaternion" : "Euler angle").c_str());
                printf("Press ENTER to continue. Press ENTER to then stop\n");
                getchar();

                std::atomic_bool stopDump;
                stopDump.store(false);
                std::thread dumpThread([&stopDump, useQuaternions] () {
                    auto ahrsDataSource = utility::Register::Factory().CreateAHRSDataSource();

                    auto pSerialLogger = utility::Register::LoggerFactory().CreateLogger("SerialLogger");
                    sbrcontroller::coms::LinuxSerialDevice serialDevice(pSerialLogger, "/dev/ttyS0");

                    while (stopDump == false)
                    {
                        auto currOriQ = ahrsDataSource->ReadOrientation();
                        std::string datum;
                        if (!useQuaternions) {
                            auto currOrientation = currOriQ.ToEuler();
                            datum = fmt::format("Orientation: {:3.2f}, {:3.2f}, {:3.2f}\n", currOrientation.GetRollInDegrees(), currOrientation.GetPitchInDegrees(), currOrientation.GetYawInDegrees());
                        } else {
                            datum = fmt::format("Quaternion: {:3.2f}, {:3.2f}, {:3.2f}, {:3.2f}\n", currOriQ.w, currOriQ.x, currOriQ.y, currOriQ.z);
                        }
                        
                        serialDevice.Write((char*)datum.c_str(), datum.size());
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    }                
                });
                getchar();
                stopDump.store(true);
                dumpThread.join();
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


