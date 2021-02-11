#include <iostream>
#include <memory>
#include <unistd.h>
#include "Register.h"
#include "FileConfigProvider.h"
#include "JSONConfig.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace sbrcontroller;

int main()
{
    try {
        // production startup process
        utility::Register::RegisterConfigProvider(
            make_shared<utility::JSONConfig>(
                make_shared<utility::FileConfigProvider>("./sbrconfig.json")
        ));
        auto pLoggerFactory = make_shared<utility::LoggerFactory>();
        auto pFactory = make_shared<utility::SBRProdFactory>();
        utility::Register::RegisterLoggerFactory(pLoggerFactory);
        utility::Register::RegisterFactory(pFactory);
        auto logger = pLoggerFactory->CreateLogger("RootLogger");
        logger->info("SBRController running!");

        auto ahrsDataSource = utility::Register::Factory().CreateAHRSDataSource();

        for (int i = 0; i < 100; i++) {
            auto currOrientation = ahrsDataSource->ReadOrientation();
            logger->debug("X rotation {} degrees, Y rotation {} degrees", currOrientation.GetRollInDegrees(), currOrientation.GetPitchInDegrees());
            usleep(200 * 1000);
            // seem to be drifting quite a bit (+/- 1degree) even when still. is this
            // sensor noise?
            // https://wired.chillibasket.com/2015/01/calibrating-mpu6050/ has some stuff about
            // fifo. calibrate? read from mp fifo?
            // raw signal synchronisation issue?? https://electronics.stackexchange.com/questions/333174/mpu6050-accel-gyro-noise-that-behaves-strangely-what-might-be-doing-this
        }
    }
    catch (const std::exception& ex) {
        cout << ex.what() << endl;
    }
}

//https://code.visualstudio.com/docs/languages/cpp

//http://wiringpi.com/reference/i2c-library/

//https://sudonull.com/post/97828-Ruby-and-C-Part-4-Friends-of-the-accelerometer-gyroscope-and-range-finder-with-Raphaeljs


//https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing ??

