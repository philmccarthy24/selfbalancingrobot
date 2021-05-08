#pragma once
#include <string>
#include <vector>
#include <memory>

namespace sbrcontroller {
    namespace motor {

        /**
         * Abstracts raw control of the motors through the
         * motor controller hardware.
         **/
        class IMotorController
        {
        public:
            IMotorController() {}
            virtual ~IMotorController() {}

            virtual std::vector<std::string> ListMotors() const = 0;
            virtual void SetMotorVelocity(const std::string& motorName, float motorVelocity) = 0;
            virtual float ReadMotorVelocity(const std::string& motorName) const = 0;
        };
    }
}