#pragma once
#include "IConfigProvider.h"
#include <string>

namespace sbrcontroller {
    namespace utility {

        class JSONConfigProvider : public IConfigProvider
        {
        public:
            JSONConfigProvider(const std::string& jsonFile);
            virtual ~JSONConfigProvider();

            virtual std::string GetConfigValue(const std::string& configKey) const override;
            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const override;
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) override;

        private:
            std::string m_jsonFile;
            std::string m_cachedConfig;
        };
    }
}