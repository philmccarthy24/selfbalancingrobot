#pragma once
#include "ISBRFactory.h"
#include "ILoggerFactory.h"
#include "IConfigSection.h"

#include <memory>

namespace sbrcontroller {
    namespace utility {

        class Register
        {
        public:
            virtual ~Register();

            static void RegisterFactory(std::shared_ptr<ISBRFactory> pFactory);
            static void RegisterLoggerFactory(std::shared_ptr<ILoggerFactory> pLoggerFactory);
            static void RegisterConfigProvider(std::shared_ptr<IConfigSection> pConfigProvider);
            
            static const ISBRFactory& Factory();
            static const ILoggerFactory& LoggerFactory();
            static IConfigSection& Config();

        private:
            Register();

            static std::shared_ptr<ISBRFactory> m_pFactory;
            static std::shared_ptr<ILoggerFactory> m_pLoggerFactory;
            static std::shared_ptr<IConfigSection> m_pConfig;
        };
    }
}

