#pragma once
#include <memory>
#include <string>
#include <vector>

namespace sbrcontroller {
    namespace utility {

        class IConfigSection
        {
        public:
            IConfigSection() {}
            virtual ~IConfigSection() {}

            virtual std::string GetConfigValue(const std::string& configKey) const = 0;
            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const = 0;
            
            // for an array of json objects (nested config sections) - eg for runtime sensor or algorithm config
            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) = 0;

            // gets sub-sections of config
            virtual std::vector<std::shared_ptr<IConfigSection>> GetConfigSections(const std::string& configKey) const = 0;
        };
    }
}