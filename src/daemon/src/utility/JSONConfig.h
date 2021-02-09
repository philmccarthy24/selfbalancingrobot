#pragma once
#include "IConfigSection.h"
#include "IConfigProvider.h"
#include <memory>

namespace sbrcontroller {
    namespace utility {

        class JSONConfig : public IConfigSection
        {
        public:
            JSONConfig(std::shared_ptr<IConfigProvider> configSource);
            virtual ~JSONConfig();

            virtual std::string GetConfigValue(const std::string& configKey) const override;
            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const override;
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) override;

            virtual std::vector<std::shared_ptr<IConfigSection>> GetConfigSections(const std::string& configKey) const override;

        private:
            std::shared_ptr<IConfigSection> m_pImpl;
        };
    }
}