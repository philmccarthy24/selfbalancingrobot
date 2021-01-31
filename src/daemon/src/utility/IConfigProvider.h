#pragma once
#include <string>
#include <vector>

namespace sbrcontroller {
    namespace utility {

        class IConfigProvider
        {
        public:
            IConfigProvider() {}
            virtual ~IConfigProvider() {}

            virtual std::string GetConfigValue(const std::string& configKey) = 0;

            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) = 0;
            // TODO: Consider if a vector<std::shared_ptr<IConfigSection>> GetConfigSections is needed
            // for an array of json objects (nested config sections) - eg for runtime sensor or algorithm config
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) = 0;
        };
    }
}