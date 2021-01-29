#pragma once
#include "ISBRFactory.h"
#include "IConfigSection.h"

#include <memory>

namespace sbrcontroller {
    namespace utility {

        class Register
        {
        public:
            virtual ~Register();

            static void RegisterFactory(std::shared_ptr<ISBRFactory> pFactory);
            static const ISBRFactory& Factory();
            static const IConfigSection& RootConfig();

        private:
            Register();

            static std::shared_ptr<ISBRFactory> m_pFactory;
            static std::shared_ptr<IConfigSection> m_pRootConfig;
        };
    }
}

