#include "SBRProdFactory.h"
#include "AHRSManager.h"
#include "Register.h"
#include "IConfigSection.h"
#include "SBRCommon.h"
#include "sensors.h"
#include "Madgwick.h"
#include "MPU6050Accel.h"
#include "MPU6050Gyro.h"
#include "FXAS21002Gyro.h"
#include "FXOS8700Accel.h"
#include "FXOS8700Mag.h"
#include "LinuxI2CDevice.h"
#include "LinuxSerialDevice.h"
#include "StringReaderWriter.h"
#include "GCodeChecksumCalc.h"
#include "ODriveController.h"
#include "SBRController.h"
#include "ILoggerFactory.h"

using namespace sbrcontroller::sensors;
using namespace sbrcontroller::coms;
using namespace sbrcontroller::ahrs;
using namespace sbrcontroller::motor;
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

        std::shared_ptr<ISerialDevice> SBRProdFactory::CreateSerialDevice(const std::string& serialDeviceName, int baudRate) const
        {
            auto pLogger = Register::LoggerFactory().CreateLogger("LinuxSerialDevice");
            return std::make_shared<LinuxSerialDevice>(pLogger, serialDeviceName, baudRate);
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

            auto fusionAlgorithmConfig = Register::Config().GetConfigSection(AHRS_FUSION_ALGORITHM_CONFIG_KEY);
            auto algorithmIdentifier = fusionAlgorithmConfig->GetConfigValue("id");

            if (algorithmIdentifier == "Madgwick")
            {
                float betaConstant = std::stof(fusionAlgorithmConfig->GetConfigValue("beta"));
                pFusionAlgorithm = make_shared<ahrs::algorithms::Madgwick>(sensorSamplePeriodHz, betaConstant);
            }
            else
            {
                throw errorhandling::NotImplementedException("Code does not yet exist to create algorithm " + algorithmIdentifier + " specified.");
            }
            return pFusionAlgorithm;
        }

        std::shared_ptr<ISensor> SBRProdFactory::CreateSensor(std::shared_ptr<utility::IConfigSection> sensorConfig) const
        {
            auto sensorId = sensorConfig->GetConfigValue("id");
            std::shared_ptr<ISensor> pSensor = nullptr;
            if (sensorId == "FXAS2100_gyro") {
                // get the stored calibration
                auto calConfig = sensorConfig->GetConfigSection("calibration");
                auto calData = GetCalibrationData(calConfig);
                auto pI2CDevice = CreateI2CDevice(FXAS2100Gyro::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("FXAS2100Gyro");
                pSensor = std::make_shared<FXAS2100Gyro>(pI2CDevice, calData, pLogger);
            } else if (sensorId == "FXOS8700_accel") {
                // get the stored calibration
                auto calConfig = sensorConfig->GetConfigSection("calibration");
                auto calData = GetCalibrationData(calConfig);
                auto pI2CDevice = CreateI2CDevice(FXOS8700Accel::I2C_ADDR);
                auto pLogger = Register::LoggerFactory().CreateLogger("FXOS8700Accel");
                pSensor = std::make_shared<FXOS8700Accel>(pI2CDevice, calData, pLogger);
            } else if (sensorId == "FXOS8700_mag") {
                // get the stored hard iron offset
                auto calConfig = sensorConfig->GetConfigSection("calibration");
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

        std::shared_ptr<IStringReaderWriter> SBRProdFactory::CreateStringReaderWriter(std::shared_ptr<coms::ISerialDevice> pSerialDevice) const
        {
            return std::make_shared<coms::StringReaderWriter>(pSerialDevice);
        }

        std::shared_ptr<IMotorController> SBRProdFactory::CreateMotorController() const
        {
            auto motorControlComsConfigSection = utility::Register::Config().GetConfigSection(MOTOR_CONTROL_COMS_KEY);
            if (motorControlComsConfigSection->GetConfigValue("type") != "serial") 
                throw errorhandling::ConfigurationException("Expecting serial port setup info in motor control coms section");

            auto serialPort = motorControlComsConfigSection->GetConfigValue("serialPort");
            int baudRate = std::stoi(motorControlComsConfigSection->GetConfigValue("baud"));
            
            auto pRawSerial = CreateSerialDevice(serialPort, baudRate);
            auto pStringReaderWriter = CreateStringReaderWriter(pRawSerial);
            auto pChecksumCalculator = CreateChecksumCalculator();
        
            auto motorConfigSections = utility::Register::Config().GetConfigSections(MOTOR_CONTROL_MOTORS_KEY);
            return std::make_shared<ODriveController>(pStringReaderWriter, pChecksumCalculator, motorConfigSections);
        }

        std::shared_ptr<motor::ISBRController> SBRProdFactory::CreateSBRController() const
        {
            float Kp = std::stof(utility::Register::Config().GetConfigValue(SELF_BALANCING_P_CONST_KEY));
            float Ki = std::stof(utility::Register::Config().GetConfigValue(SELF_BALANCING_I_CONST_KEY));
            float Kd = std::stof(utility::Register::Config().GetConfigValue(SELF_BALANCING_D_CONST_KEY));
            float velocityLimit = std::stof(utility::Register::Config().GetConfigValue(SELF_BALANCING_VELOCITY_LIMIT_KEY));
            float targetTiltAngle = std::stof(utility::Register::Config().GetConfigValue(SELF_BALANCING_TARGET_TILT_KEY));

            auto pLogger = utility::Register::LoggerFactory().CreateLogger("SBRController");
            
            auto pMotorController = CreateMotorController();
            auto pAHRSSource = CreateAHRSDataSource();
            
            return std::make_shared<motor::SBRController>(pLogger, pAHRSSource, pMotorController, Kp, Ki, Kd, velocityLimit, targetTiltAngle);
        }
    }
}