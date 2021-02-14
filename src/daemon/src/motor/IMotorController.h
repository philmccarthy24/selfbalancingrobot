#pragma once
#include <string>
#include <vector>
#include <memory>

namespace sbrcontroller {
    namespace motor {

        class IMotor
        {
        public:
            IMotor() {}
            virtual ~IMotor() {}

            virtual std::string GetIdentifier() = 0;
            virtual float GetTorque() = 0;
            virtual float GetVelocity() = 0;
            virtual void SetTorque(float torque) = 0;
            virtual void SetVelocity(float velocity) = 0;
        };

        class IMotorController
        {
        public:
            IMotorController() {}
            virtual ~IMotorController() {}

            virtual std::vector<std::string> ListMotors() = 0;
            virtual std::shared_ptr<IMotor> GetMotor(const std::string& motorIdentifier) = 0;
        };
    }
}