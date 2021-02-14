#pragma once

#include "IMotorController.h"
#include "ISerialComs.h"
#include <string>
#include <vector>
#include <memory>

namespace sbrcontroller {
    namespace motor {

        class ODriveController : public IMotorController
        {
        public:
            ODriveController(std::shared_ptr<ISerialComs> pSerialComs);
            virtual ~ODriveController();

            virtual std::vector<std::string> ListMotors() override;
            virtual std::shared_ptr<IMotor> GetMotor(const std::string& motorIdentifier) override;

        private:
            std::shared_ptr<IMotorController> m_pImpl;
        };
    }
}