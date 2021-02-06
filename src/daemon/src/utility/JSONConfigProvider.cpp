#include "JSONConfigProvider.h"
#include "sbrcontroller.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <streambuf>
#include <vector>
#include <string>

using json = nlohmann::json;

namespace sbrcontroller {
    namespace utility {

        JSONConfigProvider::JSONConfigProvider(const std::string& jsonFile) :
            m_jsonFile(jsonFile)
        {
            std::ifstream ifs(m_jsonFile);
            std::string str((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());
            m_cachedConfig = str;
        }

        JSONConfigProvider::~JSONConfigProvider()
        {
        }

        std::string JSONConfigProvider::GetConfigValue(const std::string& configKey) const
        {
            std::string configValue;

            try
            {
                auto jp = json::json_pointer(configKey);
                auto configJson = json::parse(m_cachedConfig);
                auto jStr = configJson[jp];
                configValue = jStr.get<std::string>();
            }
            catch (json::parse_error& e)
            {
                throw errorhandling::ParseException("Error parsing config json");
            }
            return configValue;
        }

        std::vector<std::string> JSONConfigProvider::GetConfigListValue(const std::string& configKey) const
        {
            std::vector<std::string> listValue;
            try
            {
                auto jp = json::json_pointer(configKey);
                auto configJson = json::parse(m_cachedConfig);
                auto jArray = configJson[jp];
                if (!jArray.is_array())
                {
                    throw errorhandling::ParseException("Expected config entry " + configKey + " to be list");
                }
                for (auto& i : jArray) {
                    listValue.push_back(i.get<std::string>());
                }
            }
            catch (json::parse_error& e)
            {
                throw errorhandling::ParseException("Error parsing config json");
            }
            return listValue;
        }

        void JSONConfigProvider::SetConfigValue(const std::string& configKey, const std::string& configValue)
        {
            //std::ofstream o("pretty.json");
            //o << std::setw(4) << j << std::endl;
        }

    }
}