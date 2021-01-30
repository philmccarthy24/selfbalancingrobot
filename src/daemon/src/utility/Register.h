#pragma once
#include "ISBRFactory.h"
#include "IConfigProvider.h"

#include <memory>

namespace sbrcontroller {
    namespace utility {

        class Register
        {
        public:
            virtual ~Register();

            static void RegisterFactory(std::shared_ptr<ISBRFactory> pFactory);
            static void RegisterConfigProvider(std::shared_ptr<IConfigProvider> pConfigProvider);
            static const ISBRFactory& Factory();
            static const IConfigProvider& Config();

        private:
            Register();

            static std::shared_ptr<ISBRFactory> m_pFactory;
            static std::shared_ptr<IConfigProvider> m_pConfig;
        };
    }
}

