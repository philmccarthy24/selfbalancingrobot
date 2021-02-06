#include "Register.h"

namespace sbrcontroller {
    namespace utility {

        std::shared_ptr<ISBRFactory> Register::m_pFactory;
        std::shared_ptr<IConfigProvider> Register::m_pConfig;

        Register::Register()
        {
        }

        Register::~Register()
        {
        }

        void Register::RegisterConfigProvider(std::shared_ptr<IConfigProvider> pConfigProvider)
        {
            std::swap(m_pConfig, pConfigProvider);
        }

        void Register::RegisterFactory(std::shared_ptr<ISBRFactory> pFactory)
        {
            std::swap(m_pFactory, pFactory);
        }
        
        const ISBRFactory& Register::Factory()
        {
            return *m_pFactory;
        }
        
        IConfigProvider& Register::Config()
        {
            return *m_pConfig;
        }
    
    }
}

