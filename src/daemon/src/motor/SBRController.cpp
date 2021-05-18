#include "SBRController.h"
#include "IMotorController.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <thread>
#include <memory>
#include <chrono>
#include <cmath>

namespace sbrcontroller {
    namespace motor {

    class SBRControllerImpl : public ISBRController, public ahrs::IAHRSDataSubscriber, public std::enable_shared_from_this<SBRControllerImpl>
    {
    public:
        SBRControllerImpl(std::shared_ptr<spdlog::logger> pLogger, std::shared_ptr<ahrs::IAHRSDataSource> pAHRSSource, std::shared_ptr<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngleDegs) :
            m_pLogger(pLogger),
            m_pAHRSSource(pAHRSSource),
            m_pMotorController(pMotorController),
            m_Kp(Kp),
            m_Ki(Ki),
            m_Kd(Kd),
            m_velocityLimit(velocityLimit),
            m_targetTiltAngleRads(targetTiltAngleDegs * (M_PI / 180)),
            m_timeDelta(100), // 100 ms / 10 times a second seems reasonable to be adjusting wheel velocities
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
            m_lastLoopTime = std::chrono::high_resolution_clock::now();
            m_pAHRSSource->Register("SBRController", shared_from_this(), m_timeDelta);
        }

        virtual void EndControl() override
        {
            m_pAHRSSource->Unregister("SBRController");
        }

        // this gets fired from an ahrs publish thread every 100ms
        virtual void OnUpdate(const ahrs::Quaternion& orientation) override
        {
            auto updateTime = std::chrono::high_resolution_clock::now();
            auto totalElapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(updateTime - m_lastLoopTime);
            float timeDeltaSecs = (float)totalElapsedUS.count() / 1000000;
            m_lastLoopTime = updateTime;
            
            auto currentOri = orientation.ToEuler();

            // Calculate error. We use sine function to give a non-linear response curve
            // proportional to the component mass of the robot that is subject to gravity
            // and therefore needs torque to counteract.
            float error = sin(m_targetTiltAngleRads) - sin(currentOri.roll);

            // Proportional term
            float Pout = m_Kp * error;

            // Integral term
            m_integral += error * timeDeltaSecs;
            float Iout = m_Ki * m_integral;

            // Derivative term
            float derivative = (error - m_prevError) / timeDeltaSecs;
            float Dout = m_Kd * derivative;

            // Calculate total output (negative because we want to counter the error)
            float newVelocity = -(Pout + Iout + Dout);

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

            m_pLogger->info("PID control loop: Pout={}, Iout={}, Dout={}, time delta {}, error {}, set V to {}", Pout, Iout, Dout, timeDeltaSecs, error, newVelocity);
        }

    private:
        std::shared_ptr<spdlog::logger> m_pLogger;
        std::shared_ptr<ahrs::IAHRSDataSource> m_pAHRSSource;
        std::shared_ptr<motor::IMotorController> m_pMotorController;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastLoopTime;

        float m_Kp;
        float m_Ki;
        float m_Kd;
        float m_velocityLimit;
        float m_targetTiltAngleRads;
        int m_timeDelta;
        float m_integral;
        float m_prevError;
    };

///////////////////////////////////////////////////////////////////////////
/////// PIMPL forwarders
///////////////////////////////////////////////////////////////////////////

    SBRController::SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared_ptr<ahrs::IAHRSDataSource> pAHRSSource, std::shared_ptr<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngleDegs)
    {
        m_pImpl = std::make_shared<SBRControllerImpl>(pLogger, pAHRSSource, pMotorController, Kp, Ki, Kd, velocityLimit, targetTiltAngleDegs);
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