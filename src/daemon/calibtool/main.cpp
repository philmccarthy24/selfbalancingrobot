#include "Register.h"
#include "FileConfigProvider.h"
#include "JSONConfig.h"
#include "sbrcontroller.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include <iostream>
#include <memory>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <fmt/core.h>

using namespace std;
using namespace sbrcontroller;

/********************
 * 
 * Rationale for this is: we need a way to get up and running with AHRS sensor calibration. for now, use
 * a standalone tool that is hard coded for the mag and gyro sensors we're actually using.
 * Put calibration for these sensors in as a runtime config setting.
 * If in future we want to support a more fancy calibration from within the running system (which would
 * require stopping polling of sensors probably, and solving writing to config), can do that if it's desirable.
 * There are probably more useful things to get finished.
 * 
 ***********************/

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

            // create sensors from config
            std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>> sensors;
            auto sensorConfig = utility::Register::Config().GetConfigSections(sbrcontroller::AHRS_SENSOR_IDS_KEY);
            for (auto& sensorConfig : sensorConfig)
            {
                sensors.push_back(pFactory->CreateSensor(sensorConfig));
            }

            // ok let's get the gyro and magnetometer to calibrate
            std::shared_ptr<sbrcontroller::sensors::ISensor> gyro = nullptr;
            std::shared_ptr<sbrcontroller::sensors::ISensor> mag = nullptr;
            for (auto& sensor : sensors) 
            {
                auto sensorInfo = sensor->GetDeviceInfo();
                if (sensorInfo.identifier == "FXAS2100Gyro Gyroscope device")
                {
                    gyro = sensor;
                    gyro->ClearCalibration();
                }
                else if (sensorInfo.identifier == "FXOS8700 Magnetometer device")
                {
                    mag = sensor;
                    mag->ClearCalibration();
                }
            }

            // first let's calibrate the gyro at rest.
            
            printf("Gyro calibration\n");
            printf("Place your gyro on a FLAT stable surface\n");
            printf("Press ENTER to continue");
            getchar();

            sbrcontroller::sensors::TripleAxisData gyroReading = {};
            float minX = 0, maxX = 0;
            float minY = 0, maxY = 0;
            float minZ = 0, maxZ = 0;

            for (int i = 0; i < 500; i++)
            {
                gyro->ReadSensorData(reinterpret_cast<unsigned char*>(&gyroReading), sizeof(sbrcontroller::sensors::TripleAxisData));

                printf(fmt::format("Gyro: x={}, y={}, z={}\n", gyroReading.x, gyroReading.y, gyroReading.z).c_str());

                minX = std::min(minX, gyroReading.x);
                minY = std::min(minY, gyroReading.y);
                minZ = std::min(minZ, gyroReading.z);
                maxX = std::max(maxX, gyroReading.x);
                maxY = std::max(maxY, gyroReading.y);
                maxZ = std::max(maxZ, gyroReading.z);

                float offset_x = (minX + maxX) / 2;
                float offset_y = (minY + maxY) / 2;
                float offset_z = (minZ + maxZ) / 2;

                printf(fmt::format("Zero Rate Offset: x={}, y={}, z={}\n", offset_x, offset_y, offset_z).c_str());

                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            printf("\nPress Return to quit\n");  
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


