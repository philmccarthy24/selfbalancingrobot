#pragma once
#include <string>

namespace sbrcontroller {
    namespace utility {

        class IConfigProvider
        {
        public:
            IConfigProvider() {}
            virtual ~IConfigProvider() {}

            virtual std::string GetConfig() const = 0;
            virtual void StoreConfig(const std::string& serialisedConfig) = 0;
        };
    }
}