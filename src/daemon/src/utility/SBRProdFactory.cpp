#include "SBRProdFactory.h"
#include "Register.h"
#include "IConfigProvider.h"
#include "sbrcontroller.h"
#include "Madgwick.h"
#include "MPU6050Accel.h"
#include "MPU6050Gyro.h"
#include "Pi4I2CDevice.h"
#include "AHRSController.h"

namespace sbrcontroller {
    namespace utility {

            SBRProdFactory::SBRProdFactory()
            {
            }

            SBRProdFactory::~SBRProdFactory()
            {
            }

            std::shared_ptr<coms::II2CDevice> SBRProdFactory::CreateI2CDevice(int deviceId)
            {
                return make_shared<coms::Pi4I2CDevice>(deviceId);
            }

            std::shared_ptr<ahrs::IAHRSDataSource> SBRProdFactory::CreateAHRSDataSource()
            {
                auto fusionAlgorithm = CreateFusionAlgorithm();
                
                std::vector<std::shared_ptr<ahrs::ISensor>> sensors;
                auto sensorIds = Register::Config().GetConfigValues(AHRS_SENSOR_IDS_KEY);
                for (auto sensorId : sensorIds)
                {
                    sensors.push_back(CreateSensor(sensorId));
                }

                int sensorSamplePeriodHz = std::stoi(Register::Config().GetConfigValue(AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY));
                return std::make_shared<ahrs::IAHRSDataSource>(fusionAlgorithm, sensors, sensorSamplePeriodHz);
            }

            std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> SBRProdFactory::CreateFusionAlgorithm()
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

            std::shared_ptr<ahrs::ISensor> SBRProdFactory::CreateSensor(const std::string& config)
            {
                std::shared_ptr<ahrs::ISensor> pSensor = nullptr;
                if (config == "MPU6050_gyro") {
                    auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                    pSensor = std::make_shared<MPU6050Gyro>(pI2CDevice);
                } else if (config == "MPU6050_gyro") {
                    auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                    pSensor = std::make_shared<MPU6050Accel>(pI2CDevice);
                } else {
                    throw errorhandling::NotImplementedException("Code does not yet exist to create sensor " + config + " specified.");
                }
                return pSensor;
            }