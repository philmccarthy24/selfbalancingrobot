#include "SBRController.h"
#include "IMotorController.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include <atomic>

namespace sbrcontroller {

class SBRControllerImpl : public ISBRController
    {
    public:
        SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController) :
            m_pLogger(pLogger),
            m_pAHRSSource(pAHRSSource),
            m_pMotorController(pMotorController)
        {
        }

        virtual ~SBRController()
        {
        }

        virtual void BeginControl() override
        {
        }

        virtual void EndControl() override
        {
        }

    private:
        std::shared_ptr<spdlog::logger> m_pLogger;
        std::shared<ahrs::IAHRSDataSource> m_pAHRSSource;
        std::shared<motor::IMotorController> m_pMotorController;

        std::atomic_bool m_bStopSignal;
    }

///////////////////////////////////////////////////////////////////////////
/////// PIMPL forwarders
///////////////////////////////////////////////////////////////////////////

    SBRController::SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController)
    {
        m_pImpl = std::make_shared<SBRControllerImpl>(pLogger, pAHRSSource, pMotorController);
    }

    SBRController::~SBRController()
    {
    }

    void SBRController::BeginControl()
    {
        m_pImpl->BeginControl();
    }

    void SBRController::EndControl()
    {
        m_pImpl->EndControl();
    }

}