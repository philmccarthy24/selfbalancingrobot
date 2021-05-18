#pragma once

#include "ISBRController.h"
#include "AHRS.h"
#include <memory>

namespace spdlog {
    class logger;
}

namespace sbrcontroller {

    namespace ahrs {
        class IAHRSDataSource;
    }

    namespace motor {

        class IMotorController;

        class SBRController : public ISBRController, public ahrs::IAHRSDataSubscriber
        {
        public:
            SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared_ptr<ahrs::IAHRSDataSource> pAHRSSource, std::shared_ptr<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngleDegs);
            virtual ~SBRController();

            virtual void OnUpdate(const ahrs::Quaternion& orientation) override;
            virtual void BeginControl() override;
            virtual void EndControl() override;

        private:
            std::shared_ptr<ISBRController> m_pImpl;
        };
    }
}