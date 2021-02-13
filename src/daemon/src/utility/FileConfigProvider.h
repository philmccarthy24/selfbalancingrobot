#pragma once
#include "IConfigProvider.h"

namespace sbrcontroller {
    namespace utility {

        class FileConfigProvider : public IConfigProvider
        {
        public:
            FileConfigProvider(const std::string& filePath);
            virtual ~FileConfigProvider();

            virtual std::string GetConfig() const override;
            virtual void StoreConfig(const std::string& serialisedConfig) override;

        private:
            std::string m_filePath;
        };
    }
}