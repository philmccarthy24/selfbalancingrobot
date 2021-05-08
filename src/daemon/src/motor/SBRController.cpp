#include "SBRController.h"
#include "IMotorController.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <thread>

namespace sbrcontroller {

class SBRControllerImpl : public ISBRController
    {
    public:
        SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngle) :
            m_pLogger(pLogger),
            m_pAHRSSource(pAHRSSource),
            m_pMotorController(pMotorController),
            m_bKillSignal(ATOMIC_VAR_INIT(false)),
            m_Kp(Kp),
            m_Ki(Ki),
            m_Kd(Kd),
            m_velocityLimit(velocityLimit),
            m_targetTiltAngle(targetTiltAngle)
        {
        }

        virtual ~SBRController()
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
            // kick off the PID thread
            m_tPIDControllerThread = std::thread([this] {PIDControllerThreadProc();});
        }

        virtual void EndControl() override
        {
            m_bKillSignal.store(true);
            m_tPIDControllerThread.join();
        }

        void PIDControllerThreadProc()
        {
            float setPoint = 0.0f;
            float wheelVelocity = 0.0f;
            // 200ms time delta? 5 times a second seems reasonable to be adjusting wheel velocities?

            while (!m_bKillSignal.load())
            {
                // take a reading from ahrs sensor
                auto q = pAHRSSource->ReadOrientation();
                auto ori = q.ToEuler();
                float currentRollAngle = ori.GetRollInDegrees();

                float error = m_targetTiltAngle - currentRollAngle;
                float p = error;
                float i = 


/* from wikipedia:                error := setpoint − measured_value
    proportional := error;
    integral := integral + error × dt
    derivative := (error − previous_error) / dt
    output := Kp × proportional + Ki × integral + Kd × derivative
    previous_error := error
    wait(dt)
    goto loop
    */

   // maybe check out https://gist.github.com/bradley219/5373998 ::
   // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
            }
        }

    private:
        std::shared_ptr<spdlog::logger> m_pLogger;
        std::shared<ahrs::IAHRSDataSource> m_pAHRSSource;
        std::shared<motor::IMotorController> m_pMotorController;

        std::thread m_tPIDControllerThread;
        std::atomic_bool m_bKillSignal;

        float m_Kp;
        float m_Ki;
        float m_Kd;
        float m_velocityLimit;
        float m_targetTiltAngle;
    }

///////////////////////////////////////////////////////////////////////////
/////// PIMPL forwarders
///////////////////////////////////////////////////////////////////////////

    SBRController::SBRController(std::shared_ptr<spdlog::logger> pLogger, std::shared<ahrs::IAHRSDataSource> pAHRSSource, std::shared<motor::IMotorController> pMotorController, float Kp, float Ki, float Kd, float velocityLimit, float targetTiltAngle)
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

}