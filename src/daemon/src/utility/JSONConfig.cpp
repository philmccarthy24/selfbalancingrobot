#include "JSONConfig.h"
#include "sbrcontroller.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <string>

using json = nlohmann::json;

namespace sbrcontroller {
    namespace utility {

        class JSONObjectPointer;

        /***
         * Pimpl pattern to isolate choice of json lib, keeping it easily swappable.
         * 
         * Inner JSONConfigImpl implementation class
         */
        class JSONConfigImpl : public IConfigSection, std::enable_shared_from_this<JSONConfigImpl>
        {
        public:
            JSONConfigImpl(std::shared_ptr<IConfigProvider> configSource) :
                m_pConfigSource(configSource)
            {
                m_cachedConfig = json::parse(m_pConfigSource->GetConfig());
            }

            virtual ~JSONConfigImpl()
            {
            }

            /*****
             * Gets a single config value from an absolute config key path
             ****/
            virtual std::string GetConfigValue(const std::string& configKey) const override
            {
                std::string configValue;
                try
                {
                    auto jp = json::json_pointer(configKey);
                    auto jStr = m_cachedConfig[jp];
                    if (jStr.is_array() || jStr.is_object()) 
                    {
                        throw errorhandling::ParseException("Expected config value not array or object");
                    }
                    configValue = jStr.is_string() ? jStr.get<std::string>() : jStr.dump();
                }
                catch (json::parse_error& e)
                {
                    throw errorhandling::ParseException("Error getting config with json path " + configKey);
                }
                return configValue;
            }

            /*****
             * Gets a list of config values from an absolute config key path
             ****/
            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const override
            {
                std::vector<std::string> listValue;
                try
                {
                    auto jp = json::json_pointer(configKey);
                    auto jArray = m_cachedConfig[jp];
                    if (!jArray.is_array())
                    {
                        throw errorhandling::ParseException("Expected config entry " + configKey + " to be list");
                    }
                    for (auto& i : jArray) 
                    {
                        if (i.is_array() || i.is_object()) 
                        {
                            throw errorhandling::ParseException("Expected primitive config value in list, not array or object");
                        }
                        listValue.push_back(i.is_string() ? i.get<std::string>() : i.dump());
                    }
                }
                catch (json::parse_error& e)
                {
                    throw errorhandling::ParseException("Error parsing config json");
                }
                return listValue;
            }

            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) override
            {
                // TODO: Set the config here. check type of val first so we don't overwrite eg number with string. then...
                //...
                m_pConfigSource->StoreConfig(m_cachedConfig.dump());
                // TODO: What about notifying things about config change?
            }

            virtual std::vector<std::shared_ptr<IConfigSection>> GetConfigSections(const std::string& configKey) const override
            {
                auto jp = json::json_pointer(configKey);
                auto configSectionsJSON = m_cachedConfig[jp];
                std::vector<std::shared_ptr<IConfigSection>> configSections;
                if (configSectionsJSON.is_array()) {
                    for (int i = 0; i < configSectionsJSON.size(); i++)
                    {
                        // note we need to do a const_pointer_cast because this method is const, so the this ptr is const.
                        auto configSection = std::make_shared<JSONObjectPointer>(std::const_pointer_cast<JSONConfigImpl>(shared_from_this()), fmt::format(configKey + "/{:d}/", i));
                        configSections.push_back(std::dynamic_pointer_cast<IConfigSection>(configSection));
                    }
                } else if (configSectionsJSON.is_object()) {
                    // note we need to do a const_pointer_cast because this method is const, so the this ptr is const.
                    auto configSection = std::make_shared<JSONObjectPointer>(std::const_pointer_cast<JSONConfigImpl>(shared_from_this()), configKey);
                    configSections.push_back(std::dynamic_pointer_cast<IConfigSection>(configSection));
                } else {
                    throw errorhandling::ParseException("Expected config section(s) to be array or single object");
                }
                return configSections;
            }

        private:
            std::shared_ptr<IConfigProvider> m_pConfigSource;
            nlohmann::json m_cachedConfig;
        };

        /***
         * JSONObjectPointer contains a jsonpath string to an object (typically in a
         * list eg /outer/list/3 or outer object) and a shared_ptr to parent JSONConfigImpl created using shared_from_this.
         * Lightweight object created in JSONConfigImpl::GetConfigSections call, implement IConfigSection and pass
         * calls back to parent (prepending obj path to the relative path passed to methods).
         * 
         */
        class JSONObjectPointer : public IConfigSection
        {
        public:
            JSONObjectPointer(std::shared_ptr<JSONConfigImpl> parent, const std::string& objPath) :
                m_objPath(objPath),
                m_pParent(parent)
            {
            }

            virtual ~JSONObjectPointer()
            {
            }

            virtual std::string GetConfigValue(const std::string& configKey) const override
            {
                return m_pParent->GetConfigValue(m_objPath + configKey);
            }

            virtual std::vector<std::string> GetConfigListValue(const std::string& configKey) const override
            {
                return m_pParent->GetConfigListValue(m_objPath + configKey);
            }

            virtual void SetConfigValue(const std::string& configKey, const std::string& configValue) override
            {
                m_pParent->SetConfigValue(m_objPath + configKey, configValue);
            }

            virtual std::vector<std::shared_ptr<IConfigSection>> GetConfigSections(const std::string& configKey) const override
            {
                return m_pParent->GetConfigSections(m_objPath + configKey);
            }

        private:
            std::shared_ptr<JSONConfigImpl> m_pParent;
            std::string m_objPath;
        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /////// Pimpl forwarders ///////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        JSONConfig::JSONConfig(std::shared_ptr<IConfigProvider> m_pConfigSource) :
            m_pImpl(std::make_shared<JSONConfigImpl>(m_pConfigSource))
        {
        }

        JSONConfig::~JSONConfig()
        {
        }

        std::string JSONConfig::GetConfigValue(const std::string& configKey) const
        {
            return m_pImpl->GetConfigValue(configKey);
        }

        std::vector<std::string> JSONConfig::GetConfigListValue(const std::string& configKey) const
        {
            return m_pImpl->GetConfigListValue(configKey);
        }

        void JSONConfig::SetConfigValue(const std::string& configKey, const std::string& configValue)
        {
            m_pImpl->SetConfigValue(configKey, configValue);
        }

        std::vector<std::shared_ptr<IConfigSection>> JSONConfig::GetConfigSections(const std::string& configKey) const 
        {
            return m_pImpl->GetConfigSections(configKey);
        }

    }
}