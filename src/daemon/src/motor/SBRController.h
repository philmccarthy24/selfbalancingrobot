#pragma once

#include "ISBRController.h"
#include <memory>

namespace sbrcontroller {

    namespace ahrs {
        class IAHRSDataSource;
    }

    namespace spdlog {
        class logger;
    }

    namespace motor {

        class IMotorController;

        class SBRController : public ISBRController
        {
        public:
            SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngle);
            virtual ~SBRController();

            virtual void BeginControl() override;
            virtual void EndControl() override;

        private:
            std::shared_ptr<ISBRController> m_pImpl;
        }
    }
}