#include "Register.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"

namespace sbrcontroller {
    namespace utility {

        std::shared_ptr<ISBRFactory> Register::m_pFactory;
        std::shared_ptr<IConfigSection> Register::m_pConfig;
        std::shared_ptr<ILoggerFactory> Register::m_pLoggerFactory;

        Register::Register()
        {
        }

        Register::~Register()
        {
        }

        void Register::RegisterConfigProvider(std::shared_ptr<IConfigSection> pConfigProvider)
        {
            std::swap(m_pConfig, pConfigProvider);
        }

        void Register::RegisterFactory(std::shared_ptr<ISBRFactory> pFactory)
        {
            std::swap(m_pFactory, pFactory);
        }

        void Register::RegisterLoggerFactory(std::shared_ptr<ILoggerFactory> pLoggerFactory)
        {
            std::swap(m_pLoggerFactory, pLoggerFactory);
        }
        
        const ISBRFactory& Register::Factory()
        {
            return *m_pFactory;
        }

        const ILoggerFactory& Register::LoggerFactory()
        {
            return *m_pLoggerFactory;
        }
        
        IConfigSection& Register::Config()
        {
            return *m_pConfig;
        }
    
    }
}

