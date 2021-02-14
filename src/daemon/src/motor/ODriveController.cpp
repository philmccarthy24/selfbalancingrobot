#pragma once

#include "ODriveController.h"

namespace sbrcontroller {
    namespace motor {

        class ODriveMotorHandle : public IMotor
        {
        public:
            ODriveMotorHandle() {}
            virtual ~ODriveMotorHandle() {}

            virtual std::string GetIdentifier() override
            {

            }

            virtual float GetTorque() override
            {

            }

            virtual float GetVelocity() override
            {

            }

            virtual void SetTorque(float torque)
            {

            }

            virtual void SetVelocity(float velocity)
            {

            }

        private:
            float m_fTorque; //TODO: determine whether we're able to set the torque independent of the velocity??
            float m_fVelocity;
            std::shared_ptr<ODriveControllerImpl> m_p
        }

        class ODriveControllerImpl : public IMotorController, public std::enable_shared_from_this<ODriveControllerImpl>
        {
        public:
            ODriveControllerImpl(std::shared_ptr<ISerialComs> pSerialComs);
            virtual ~ODriveControllerImpl();

            virtual std::vector<std::string> ListMotors() override;
            virtual std::shared_ptr<IMotor> GetMotor(const std::string& motorIdentifier) override;

        private:
            std::shared_ptr<IMotorController> m_pImpl;
        };


        ////////////////////////////////////////////////////////////////////////
        ////////// PIMPL Forwarders
        
        ODriveController::ODriveController(std::shared_ptr<ISerialComs> pSerialComs)
        {
            m_pImpl = make_shared<ODriveControllerImpl>(pSerialComs);
        }

        ODriveController::~ODriveController()
        {
        }

        std::vector<std::string> ODriveController::ListMotors()
        {
            return m_pImpl->ListMotors();
        }

        std::shared_ptr<IMotor> ODriveController::GetMotor(const std::string& motorIdentifier)
        {
            return m_pImpl->GetMotor(motorIdentifier);
        }

    }
}