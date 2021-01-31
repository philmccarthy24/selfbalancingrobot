#include "JSONConfigProvider.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <streambuf>

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

        std::string JSONConfigProvider::GetConfigValue(const std::string& configKey)
        {
            auto configJson = json::parse(m_cachedConfig);
            //configJson["/baz/1"] // supports jsonPointer syntax for querying
        }

        std::vector<std::string> JSONConfigProvider::GetConfigListValue(const std::string& configKey)
        {
            auto configJson = json::parse(m_cachedConfig);
            auto jArray = configJson[json::pointer(configKey)];
            
        }

        void JSONConfigProvider::SetConfigValue(const std::string& configKey, const std::string& configValue)
        {
            //std::ofstream o("pretty.json");
            //o << std::setw(4) << j << std::endl;
        }

    }
}