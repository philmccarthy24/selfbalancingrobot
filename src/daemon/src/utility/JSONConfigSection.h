#pragma once
#include "IConfigSection.h"

namespace sbrcontroller {
    namespace utility {

        class JSONConfigSection : public IConfigSection
        {
        public:
            JSONConfigSection(const std::string& jsonFile, const std::string& rootPrefix = "/");
            virtual ~JSONConfigSection();

            virtual std::string GetConfigValue(const std::string& configKey) const override;
            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const override;
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) override;

            virtual std::vector<std::shared_ptr<IConfigSection>> GetConfigSections(const std::string& configKey) const override;

        private:
            std::string m_jsonFile;
            std::string m_cachedConfig;
            std::string m_rootPrefix;
        };
    }
}