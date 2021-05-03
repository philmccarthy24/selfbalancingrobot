#pragma once

#include <memory>

namespace sbrcontroller {

    namespace motor {
        class IMotorController;
    }

    namespace ahrs {
        class IAHRSDataSource;
    }

    namespace spdlog {
        class logger;
    }

    class ISBRController
    {
    public:
        ISBRController();
        virtual ~SBRController();

        virtual void BeginControl() = 0;
        virtual void EndControl() = 0;
    }

    class SBRController : public ISBRController
    {
    public:
        SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController);
        virtual ~SBRController();

        virtual void BeginControl() override;
        virtual void EndControl() override;

    private:
        std::shared_ptr<ISBRController> m_pImpl;
    }

}