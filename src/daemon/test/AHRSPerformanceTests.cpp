#include "Register.h"
#include "MemoryConfigProvider.h"
#include "JSONConfig.h"
#include "SBRProdFactory.h"
#include "LoggerFactory.h"
#include "AHRS.h"
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

class AHRSPerformanceTests : public ::testing::Test
{
protected:
    
    const std::string mpu6050raw_madgwick_config = R"CONFIG(
{
    "ahrs": {
        "fusionAlgorithm": "Madgwick",
        "sensorSampleRateHz": 20,
        "sensors": [
            "MPU6050_gyro",
            "MPU6050_accel"
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

    const std::string fxas2100_fxos8700_raw_madgwick_config = R"CONFIG(
{
    "ahrs": {
        "fusionAlgorithm": "Madgwick",
        "sensorSampleRateHz": 20,
        "sensors": [
            "FXAS2100_gyro",
            "FXOS8700_accel",
            "FXOS8700_mag"
        ]
    },
    "logging": {
        "level": "warning",
        "pattern": "%+",
        "sinks": [
            {
                "type": "stdout_color_sink_mt"
            }
        ],
        "loggers": [
            {
                "name": "RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick",
                "level": "trace",
                "pattern": "%+"
            }
        ]
    }
})CONFIG";

    virtual void SetUp()
    {      
     
    }

    virtual void TearDown()
    { 
    }

    void RestingSensorVarianceTest(const std::string& config);
};

/*
[2021-02-13 09:28:26.749] [RootLogger] [info] SBRController running!
[2021-02-13 09:28:26.757] [RootLogger] [debug] Collecting performance data for 100 ahrs samples
[2021-02-13 09:28:26.757] [AHRSManager] [info] Starting ahrs sampling at 20Hz (50 ms sleeps)
[2021-02-13 09:29:00.719] [RootLogger] [debug] Stationary ahrs analysis for 100 samples - collection took 34.0s (should be 30s):
[2021-02-13 09:29:00.719] [RootLogger] [debug] roll mean=-0.51567, sd=1.36835, minerr=-0.89974, maxerr=5.63310
[2021-02-13 09:29:00.719] [RootLogger] [debug] pitch mean=-19.06574, sd=2.90817, minerr=-1.30235, maxerr=18.46657
[2021-02-13 09:29:00.720] [RootLogger] [debug] yaw mean=43.54378, sd=26.19835, minerr=-43.53138, maxerr=44.58223

[2021-02-13 11:56:46.532] [AHRSManager] [info] Starting ahrs sampling at 20Hz (50 ms sleeps)
[2021-02-13 11:57:20.616] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] Stationary ahrs analysis for 100 samples - collection took 34.1s (should be 30s):
[2021-02-13 11:57:20.617] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] roll mean=1.16603, sd=1.45603, minerr=-1.28715, maxerr=5.78910
[2021-02-13 11:57:20.617] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] pitch mean=-22.88082, sd=3.78553, minerr=-1.57132, maxerr=22.29497
[2021-02-13 11:57:20.617] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] yaw mean=50.78345, sd=30.97587, minerr=-50.81097, maxerr=53.07627

// after propping sensor board up a bit
[2021-02-13 11:59:44.904] [AHRSManager] [info] Starting ahrs sampling at 20Hz (50 ms sleeps)
[2021-02-13 12:00:18.972] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] Stationary ahrs analysis for 100 samples - collection took 34.1s (should be 30s):
[2021-02-13 12:00:18.972] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] roll mean=1.84588, sd=0.30032, minerr=-1.30037, maxerr=1.28994
[2021-02-13 12:00:18.972] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] pitch mean=-7.97744, sd=0.89914, minerr=-0.84855, maxerr=7.39090
[2021-02-13 12:00:18.972] [RestingSensorVarianceTest_mpu6050raw_madgwick] [info] yaw mean=19.28678, sd=11.60249, minerr=-19.30936, maxerr=19.95780

// seem to be drifting quite a bit (+/- 1degree) even when still. is this
// sensor noise?
// https://wired.chillibasket.com/2015/01/calibrating-mpu6050/ has some stuff about
// fifo. calibrate? read from mp fifo?
// raw signal synchronisation issue?? https://electronics.stackexchange.com/questions/333174/mpu6050-accel-gyro-noise-that-behaves-strangely-what-might-be-doing-this
*/
TEST_F(AHRSPerformanceTests, RestingSensorVarianceTest_mpu6050raw_madgwick)
{
    RestingSensorVarianceTest(mpu6050raw_madgwick_config);
}

/*
[2021-03-06 10:17:57.795] [RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick] [info] Stationary ahrs analysis for 100 samples - collection took 30.6s (should be 30s):
[2021-03-06 10:17:57.795] [RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick] [info] roll mean=-5.43718, sd=3.61412, minerr=-2.56607, maxerr=8.06178
[2021-03-06 10:17:57.795] [RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick] [info] pitch mean=-16.51877, sd=4.41355, minerr=-7.56726, maxerr=15.99185
[2021-03-06 10:17:57.795] [RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick] [info] yaw mean=71.53038, sd=29.94567, minerr=-71.32337, maxerr=26.02939
*/
TEST_F(AHRSPerformanceTests, RestingSensorVarianceTest_fxas2100_fxos8700raw_madgwick)
{
    RestingSensorVarianceTest(fxas2100_fxos8700_raw_madgwick_config);
}

void AHRSPerformanceTests::RestingSensorVarianceTest(const std::string& config)
{
    //TODO: can we lift this up into a common area, and then
    // modify the config in memory for each test?
    utility::Register::RegisterConfigService(
            make_shared<utility::JSONConfig>(
                make_shared<MemoryConfigProvider>(config)
        ));

    auto pLoggerFactory = make_shared<utility::LoggerFactory>();
    utility::Register::RegisterLoggerFactory(pLoggerFactory);
    auto logger = pLoggerFactory->CreateLogger(::testing::UnitTest::GetInstance()->current_test_info()->name());

    try {
        auto pFactory = make_shared<utility::SBRProdFactory>();
        utility::Register::RegisterFactory(pFactory);

        auto ahrsDataSource = utility::Register::Factory().CreateAHRSDataSource();

        const int sampleSize = 100;
        const int sleepMS = 300;
        spdlog::stopwatch sw;
        logger->info("Collecting performance data for {} ahrs samples", sampleSize);
        
        std::vector<ahrs::Ori3DRads> collectedData;
        for (int i = 0; i < sampleSize; i++) {
            auto currOrientation = ahrsDataSource->ReadOrientation().ToEuler();
            collectedData.push_back(currOrientation);
            logger->trace("Roll {:03.5f} degrees, Pitch {:03.5f} degrees, Yaw {:03.5f} degrees", currOrientation.GetRollInDegrees(), currOrientation.GetPitchInDegrees(), currOrientation.GetYawInDegrees());
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS));
        }

        logger->info("Stationary ahrs analysis for {} samples - collection took {:.3}s (should be {}s):", collectedData.size(), sw, (static_cast<float>(sleepMS)/1000.0) * sampleSize);
        double rollMean = 0.0, rollStdDev = 0.0, rollMaxErr = 0.0, rollMinErr = 0.0;
        double pitchMean = 0.0, pitchStdDev = 0.0, pitchMaxErr = 0.0, pitchMinErr = 0.0;
        double yawMean = 0.0, yawStdDev = 0.0, yawMaxErr = 0.0, yawMinErr = 0.0;
        
        for (auto& item : collectedData) {
            rollMean += item.GetRollInDegrees();
            pitchMean += item.GetPitchInDegrees();
            yawMean += item.GetYawInDegrees();
        }
        rollMean /= collectedData.size();
        pitchMean /= collectedData.size();
        yawMean /= collectedData.size();

        rollStdDev = sqrt(std::accumulate(collectedData.begin(), collectedData.end(), 0.0, [&] (double sqErrSum, const ahrs::Ori3DRads& nextItem) {
            return sqErrSum + pow(nextItem.GetRollInDegrees() - rollMean, 2);
        }) / collectedData.size());
        pitchStdDev = sqrt(std::accumulate(collectedData.begin(), collectedData.end(), 0.0, [&] (double sqErrSum, const ahrs::Ori3DRads& nextItem) {
            return sqErrSum + pow(nextItem.GetPitchInDegrees() - pitchMean, 2);
        }) / collectedData.size());
        yawStdDev = sqrt(std::accumulate(collectedData.begin(), collectedData.end(), 0.0, [&] (double sqErrSum, const ahrs::Ori3DRads& nextItem) {
            return sqErrSum + pow(nextItem.GetYawInDegrees() - yawMean, 2);
        }) / collectedData.size());

        for (auto& item : collectedData) {
            double rollErr = item.GetRollInDegrees() - rollMean;
            double pitchErr = item.GetPitchInDegrees() - pitchMean;
            double yawErr = item.GetYawInDegrees() - yawMean;
            if (rollMaxErr < rollErr) {
                rollMaxErr = rollErr;
            }
            if (rollMinErr > rollErr) {
                rollMinErr = rollErr;
            }
            if (pitchMaxErr < pitchErr) {
                pitchMaxErr = pitchErr;
            }
            if (pitchMinErr > pitchErr) {
                pitchMinErr = pitchErr;
            }
            if (yawMaxErr < yawErr) {
                yawMaxErr = yawErr;
            }
            if (yawMinErr > yawErr) {
                yawMinErr = yawErr;
            }
        }

        logger->info("roll mean={:03.5f}, sd={:03.5f}, minerr={:03.5f}, maxerr={:03.5f}", rollMean, rollStdDev, rollMinErr, rollMaxErr);
        logger->info("pitch mean={:03.5f}, sd={:03.5f}, minerr={:03.5f}, maxerr={:03.5f}", pitchMean, pitchStdDev, pitchMinErr, pitchMaxErr);
        logger->info("yaw mean={:03.5f}, sd={:03.5f}, minerr={:03.5f}, maxerr={:03.5f}", yawMean, yawStdDev, yawMinErr, yawMaxErr);
    }
    catch (const std::exception& ex) {
        logger->error(ex.what());
    }

    // TODO: Add failure thresholds?
    ASSERT_TRUE(true);

}

    }
}