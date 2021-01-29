#pragma once
#include "Register.h"

namespace sbrcontroller {
    namespace utility {

        Register::Register()
        {
        }

        Register::~Register()
        {
        }

        void Register::RegisterFactory(std::shared_ptr<ISBRFactory> pFactory)
        {
            std::swap(m_pFactory, pFactory);
            if (m_pRootConfig == nullptr) {
                m_pRootConfig = m_pFactory->CreateConfig();
            }
        }
        
        const ISBRFactory& Register::Factory()
        {
            return *m_pFactory;
        }
        
        const IConfigSection& Register::RootConfig()
        {
            return *m_pRootConfig;
        }
    
    }
}

