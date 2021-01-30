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
            virtual std::vector<std::string> GetConfigValues(const std::string& configKey) = 0;
            // TODO: Consider if a map<std::string, std::shared_ptr<IConfigSection> GetConfigSections is needed
            // for an array of objects with 'key' properties - eg for runtime sensor config
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) = 0;
        };
    }
}