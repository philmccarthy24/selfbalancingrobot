#pragma once
#include "ISBRFactory.h"
#include "sensors.h"

namespace sbrcontroller {
    namespace utility {

        class IConfigSection;

        class SBRProdFactory : public ISBRFactory
        {
        public:
            SBRProdFactory();
            virtual ~SBRProdFactory();

            virtual std::shared_ptr<coms::II2CDevice> CreateI2CDevice(int deviceId) const override;

            virtual std::shared_ptr<ahrs::IAHRSDataSource> CreateAHRSDataSource() const override;
            virtual std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> CreateFusionAlgorithm() const override;
            virtual std::shared_ptr<sbrcontroller::sensors::ISensor> CreateSensor(std::shared_ptr<sbrcontroller::utility::IConfigSection> sensorConfig) const override;

            virtual std::shared_ptr<IChecksumCalculator> CreateChecksumCalculator() const override;

        private:
            sbrcontroller::sensors::TripleAxisData GetCalibrationData(std::shared_ptr<sbrcontroller::utility::IConfigSection> calibConfig) const;
        };
    }
}