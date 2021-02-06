#include "SBRProdFactory.h"
#include "AHRSController.h"
#include "Register.h"
#include "IConfigSection.h"
#include "sbrcontroller.h"
#include "sensors.h"
#include "Madgwick.h"
#include "MPU6050Accel.h"
#include "MPU6050Gyro.h"
#include "Pi4I2CDevice.h"
#include "WiringPiWrapper.h"
#include "spdlog/spdlog.h"
#include <memory>

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
            return std::make_shared<WiringPiWrapper>(deviceId);
        }

        std::shared_ptr<ahrs::IAHRSDataSource> SBRProdFactory::CreateAHRSDataSource() const
        {
            auto fusionAlgorithm = CreateFusionAlgorithm();
            
            std::vector<std::shared_ptr<ISensor>> sensors;
            auto sensorIds = Register::Config().GetConfigListValue(AHRS_SENSOR_IDS_KEY);
            for (auto sensorId : sensorIds)
            {
                sensors.push_back(CreateSensor(sensorId));
            }

            int sensorSamplePeriodHz = std::stoi(Register::Config().GetConfigValue(AHRS_SENSOR_SAMPLE_RATE_CONFIG_KEY));
            return std::make_shared<ahrs::AHRSController>(fusionAlgorithm, sensors, sensorSamplePeriodHz);
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

        std::shared_ptr<ISensor> SBRProdFactory::CreateSensor(const std::string& config) const
        {
            std::shared_ptr<ISensor> pSensor = nullptr;
            if (config == "MPU6050_gyro") {
                auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                pSensor = std::make_shared<MPU6050Gyro>(pI2CDevice);
            } else if (config == "MPU6050_accel") {
                auto pI2CDevice = CreateI2CDevice(MPU6050_I2C_ADDR);
                pSensor = std::make_shared<MPU6050Accel>(pI2CDevice);
            } else {
                throw errorhandling::NotImplementedException("Code does not yet exist to create sensor " + config + " specified.");
            }
            return pSensor;
        }

        std::shared_ptr<spdlog::logger> SBRProdFactory::CreateLogger(const std::string& className) const
        {
            if (m_pConfig == nullptr) {
                throw errorhandling::InvalidOperationException("Initialise config before logging");
            }
        }
    }
}