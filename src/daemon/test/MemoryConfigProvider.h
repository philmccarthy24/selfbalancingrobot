#pragma once
#include "IConfigProvider.h"
#include <string>

namespace sbrcontroller {
    namespace test {

        class MemoryConfigProvider : public sbrcontroller::utility::IConfigProvider
        {
        public:
            MemoryConfigProvider(const std::string& initialValues) :
                m_config(initialValues)
            {
            }
            virtual ~MemoryConfigProvider() {}

            virtual std::string GetConfig() const override {
                return m_config;
            };

            virtual void StoreConfig(const std::string& serialisedConfig)
            {
                m_config = serialisedConfig;
            };
        private:
            std::string m_config;
        };
    }
}