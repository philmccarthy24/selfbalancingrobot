#include "SBRController.h"
#include "IMotorController.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <thread>
#include <memory>

namespace sbrcontroller {
    namespace motor {

    class SBRControllerImpl : public ISBRController, public ahrs::IAHRSDataSubscriber, public std::enable_shared_from_this<SBRControllerImpl>
    {
    public:
        SBRControllerImpl(std::shared_ptr<spdlog::logger> pLogger, std::shared_ptr<ahrs::IAHRSDataSource> pAHRSSource, std::shared_ptr<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngle) :
            m_pLogger(pLogger),
            m_pAHRSSource(pAHRSSource),
            m_pMotorController(pMotorController),
            m_Kp(Kp),
            m_Ki(Ki),
            m_Kd(Kd),
            m_velocityLimit(velocityLimit),
            m_targetTiltAngle(targetTiltAngle),
            m_timeDelta(200), // 200 ms / 5 times a second seems reasonable to be adjusting wheel velocities
            m_integral(0.0f),
            m_prevError(0.0f)
        {
        }

        virtual ~SBRControllerImpl()
        {
            try 
            {
                EndControl();
            }
            catch (const std::exception& e)
            {
            }
        }

        virtual void BeginControl() override
        {
            m_pAHRSSource->Register("SBRController", shared_from_this(), m_timeDelta);
        }

        virtual void EndControl() override
        {
            m_pAHRSSource->Unregister("SBRController");
        }

        // this gets fired from an ahrs publish thread every 200ms
        virtual void OnUpdate(const ahrs::Quaternion& orientation) override
        {
            // based on example in https://gist.github.com/bradley219/5373998
            auto ori = orientation.ToEuler();
            float currentRollAngle = ori.GetRollInDegrees();

            // Calculate error
            float error = m_targetTiltAngle - currentRollAngle;

            // Proportional term
            float Pout = m_Kp * error;

            // Integral term
            m_integral += error * m_timeDelta;
            float Iout = m_Ki * m_integral;

            // Derivative term
            float derivative = (error - m_prevError) / m_timeDelta;
            float Dout = m_Kd * derivative;

            // Calculate total output
            float newVelocity = Pout + Iout + Dout;

            // Restrict to max/min
            if( newVelocity > m_velocityLimit )
                newVelocity = m_velocityLimit;
            else if ( newVelocity < -m_velocityLimit )
                newVelocity = -m_velocityLimit;

            // Save error to previous error
            m_prevError = error;

            // set the new wheel velocities
            m_pMotorController->SetMotorVelocity("left", newVelocity);
            m_pMotorController->SetMotorVelocity("right", newVelocity);
        }

    private:
        std::shared_ptr<spdlog::logger> m_pLogger;
        std::shared_ptr<ahrs::IAHRSDataSource> m_pAHRSSource;
        std::shared_ptr<motor::IMotorController> m_pMotorController;

        float m_Kp;
        float m_Ki;
        float m_Kd;
        float m_velocityLimit;
        float m_targetTiltAngle;
        int m_timeDelta;
        float m_integral;
        float m_prevError;
    };

///////////////////////////////////////////////////////////////////////////
/////// PIMPL forwarders
///////////////////////////////////////////////////////////////////////////

    SBRController::SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared_ptr<ahrs::IAHRSDataSource> pAHRSSource, std::shared_ptr<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngle)
    {
        m_pImpl = std::make_shared<SBRControllerImpl>(pLogger, pAHRSSource, pMotorController, Kp, Ki, Kd, velocityLimit, targetTiltAngle);
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

    void SBRController::OnUpdate(const ahrs::Quaternion& orientation)
    {
        //not forwarded as not directly called
    }
    }
}