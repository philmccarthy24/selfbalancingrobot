#include "SBRProdFactory.h"
#include "AHRSManager.h"
#include "Register.h"
#include "IConfigSection.h"
#include "sbrcontroller.h"
#include "sensors.h"
#include "Madgwick.h"
#include "MPU6050Accel.h"
#include "MPU6050Gyro.h"
#include "FXAS21002Gyro.h"
#include "FXOS8700Accel.h"
#include "FXOS8700Mag.h"
#include "LinuxI2CDevice.h"
#include "GCodeChecksumCalc.h"
#include "ILoggerFactory.h"

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
            return std::make_shared<LinuxI2CDevice>(deviceId);
        }

        std::shared_ptr<ahrs::IAHRSDataSource> SBRProdFactory::CreateAHRSDataSource() const
        {
            auto fusionAlgorithm = CreateFusionAlgorithm();
            
            std::vector<std::shared_ptr<ISensor>> sensors;

            auto sensorConfigs = Register::Config().GetConfigSections(AHRS_SENSOR_IDS_KEY);
            for (auto sensorConfig : sensorConfigs)
            {
                sensors.push_back(CreateSensor(sensorConfig));
            }

            int sensorSamplePeriodHz = std::stoi(Register::Config().GetConfigValue(AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY));
            auto pLogger = Register::LoggerFactory().CreateLogger("AHRSManager");
            return std::make_shared<ahrs::AHRSManager>(fusionAlgorithm, sensors, sensorSamplePeriodHz, pLogger);
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

        std::shared_ptr<ISensor> SBRProdFactory::CreateSensor(std::shared_ptr<utility::IConfigSection> sensorConfig) const
        {
            auto sensorId = sensorConfig->GetConfigValue("id");
            std::shared_ptr<ISensor> pSensor = nullptr;
            if (sensorId == "FXAS2100_gyro") {
                // get the stored calibration
                auto calConfig = sensorConfig->GetConfigSections("calibration")[0];
                auto calData = GetCalibrationData(calConfig);
                auto pI2CDevice = CreateI2CDevice(FXAS2100Gyro::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("FXAS2100Gyro");
                pSensor = std::make_shared<FXAS2100Gyro>(pI2CDevice, calData, pLogger);
            } else if (sensorId == "FXOS8700_accel") {
                auto pI2CDevice = CreateI2CDevice(FXOS8700Accel::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("FXOS8700Accel");
                pSensor = std::make_shared<FXOS8700Accel>(pI2CDevice, pLogger);
            } else if (sensorId == "FXOS8700_mag") {
                // get the stored hard iron offset
                auto calConfig = sensorConfig->GetConfigSections("calibration")[0];
                auto calData = GetCalibrationData(calConfig);
                auto pI2CDevice = CreateI2CDevice(FXOS8700Mag::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("FXOS8700Mag");
                pSensor = std::make_shared<FXOS8700Mag>(pI2CDevice, calData, pLogger);
            } else if (sensorId == "MPU6050_gyro") {
                auto pI2CDevice = CreateI2CDevice(MPU6050Gyro::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("MPU6050Gyro");
                pSensor = std::make_shared<MPU6050Gyro>(pI2CDevice, pLogger);
            } else if (sensorId == "MPU6050_accel") {
                auto pI2CDevice = CreateI2CDevice(MPU6050Accel::I2C_ADDR);
                pSensor = std::make_shared<MPU6050Accel>(pI2CDevice);
            } else {
                throw errorhandling::NotImplementedException("Code does not yet exist to create sensor " + sensorId + " specified.");
            }
            return pSensor;
        }

        sbrcontroller::sensors::TripleAxisData SBRProdFactory::GetCalibrationData(std::shared_ptr<sbrcontroller::utility::IConfigSection> calibConfig) const
        {
            sbrcontroller::sensors::TripleAxisData calibData;
            calibData.x = std::stof(calibConfig->GetConfigValue("x"));
            calibData.y = std::stof(calibConfig->GetConfigValue("y"));
            calibData.z = std::stof(calibConfig->GetConfigValue("z"));
            return calibData;
        }

        std::shared_ptr<IChecksumCalculator> SBRProdFactory::CreateChecksumCalculator() const
        {
            return std::make_shared<GCodeChecksumCalc>();
        }

    }
}