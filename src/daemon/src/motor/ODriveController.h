#pragma once

#include "IMotorController.h"
#include <string>
#include <vector>
#include <memory>

namespace sbrcontroller {

    namespace utility {
        class IChecksumCalculator;
        class IConfigSection;
    }

    namespace coms {
        class IStringReaderWriter;
    }

    namespace motor {

        class ODriveController : public IMotorController
        {
        public:
            ODriveController(std::shared_ptr<sbrcontroller::coms::IStringReaderWriter> pStringReaderWriter, 
                std::shared_ptr<sbrcontroller::utility::IChecksumCalculator> pChecksumCalculator,
                const std::vector<std::shared_ptr<sbrcontroller::utility::IConfigSection>>& motorConfigs);
            virtual ~ODriveController();

            virtual std::vector<std::string> ListMotors() const override;
            virtual void SetMotorVelocity(const std::string& motorName, float motorVelocity) override;
            virtual float ReadMotorVelocity(const std::string& motorName) const override;

        private:
            std::shared_ptr<IMotorController> m_pImpl;
        };
    }
}