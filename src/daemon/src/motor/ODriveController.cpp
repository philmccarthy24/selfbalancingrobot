#include "ODriveController.h"
#include "IStringReaderWriter.h"
#include "IConfigSection.h"
#include "SBRCommon.h"
#include <fmt/core.h>
#include <memory>
#include <map>

using namespace sbrcontroller::coms;
using namespace sbrcontroller::utility;

namespace sbrcontroller {
    namespace motor {

        struct MotorConfig
        {
            int id;
            bool isReversed;
        };

        class ODriveControllerImpl : public IMotorController
        {
        public:
            ODriveControllerImpl(std::shared_ptr<IStringReaderWriter> pStringReaderWriter,
                const std::vector<std::shared_ptr<IConfigSection>>& motorConfigs) :
            m_pStringReaderWriter(pStringReaderWriter)
            {
                for (auto& motorConfig : motorConfigs) 
                {
                    auto pStoredMotorConfig = std::make_shared<MotorConfig>();
                    auto name = motorConfig->GetConfigValue("name");
                    pStoredMotorConfig->id = std::stoi(motorConfig->GetConfigValue("id"));
                    auto revstr = motorConfig->GetConfigValue("reversed");
                    pStoredMotorConfig->isReversed = (revstr == "true" || revstr == "True");
                    m_motorSettings[name] = pStoredMotorConfig;

                    // set motor to closed loop control mode
                    auto setClosedLoopCmd = AppendGCodeChecksum(fmt::format("w axis{:d}.requested_state 8", pStoredMotorConfig->id)); // AXIS_STATE_CLOSED_LOOP_CONTROL
                    m_pStringReaderWriter->Write(setClosedLoopCmd);
                }
            }
            
            virtual ~ODriveControllerImpl()
            {
                try
                {
                    for (const auto& motorConfig : m_motorSettings) 
                    {
                        SetMotorVelocity(motorConfig.first, 0.0f);
                        auto setIdleStateCmd = AppendGCodeChecksum(fmt::format("w axis{:d}.requested_state 1", motorConfig.second->id));
                        m_pStringReaderWriter->Write(setIdleStateCmd);
                    }
                } 
                catch (const std::exception& e)
                {
                }
            }

            virtual std::vector<std::string> ListMotors() const override
            {
                std::vector<std::string> motorNames;
                for (const auto& i : m_motorSettings)
                {
                    motorNames.push_back(i.first);
                }
                return motorNames;
            }

            void SetMotorVelocity(const std::string& motorName, float motorVelocity) override
            {
                const auto& iter = m_motorSettings.find(motorName);
                if (iter == m_motorSettings.end())
                    throw errorhandling::InvalidDeviceException("Motor with name " + motorName + " not found");
                const auto& motorConfig = iter->second;
                auto setVelocityCmd = AppendGCodeChecksum(fmt::format("v {} {} 0", motorConfig->id, motorConfig->isReversed ? -motorVelocity : motorVelocity));
                m_pStringReaderWriter->Write(setVelocityCmd);
            }

            float ReadMotorVelocity(const std::string& motorName) const override
            {
                return 0.0f;
            }

            std::string AppendGCodeChecksum(const std::string& cmd)
            {
                char cs8bit = 0;
                for (const char& c : cmd)
                {
                    cs8bit ^= c;
                }
                return fmt::format("{}*{:d}\n", cmd, cs8bit);
            }

        private:
            std::shared_ptr<IStringReaderWriter> m_pStringReaderWriter;
            std::map<std::string, std::shared_ptr<MotorConfig>> m_motorSettings;
        };


        ////////////////////////////////////////////////////////////////////////
        ////////// PIMPL Forwarders
        
        ODriveController::ODriveController(std::shared_ptr<IStringReaderWriter> pStringReaderWriter, const std::vector<std::shared_ptr<IConfigSection>>& motorConfigs)
        {
            m_pImpl = std::make_shared<ODriveControllerImpl>(pStringReaderWriter, motorConfigs);
        }

        ODriveController::~ODriveController()
        {
        }

        std::vector<std::string> ODriveController::ListMotors() const
        {
            return m_pImpl->ListMotors();
        }

        void ODriveController::SetMotorVelocity(const std::string& motorName, float motorVelocity)
        {
            m_pImpl->SetMotorVelocity(motorName, motorVelocity);
        }

        float ODriveController::ReadMotorVelocity(const std::string& motorName) const
        {
            return m_pImpl->ReadMotorVelocity(motorName);
        }

    }
}