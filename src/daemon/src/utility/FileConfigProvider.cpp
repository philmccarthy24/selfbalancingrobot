#include "FileConfigProvider.h"
#include <fstream>
#include <streambuf>

namespace sbrcontroller {
    namespace utility {

        FileConfigProvider::FileConfigProvider(const std::string& filePath) :
            m_filePath(filePath)
        {
        }

        FileConfigProvider::~FileConfigProvider()
        {
        }

        std::string FileConfigProvider::GetConfig() const
        {
            std::ifstream ifs(m_filePath);
            return std::string((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());
        }
        
        void FileConfigProvider::StoreConfig(const std::string& serialisedConfig)
        {
            std::ofstream ofs(m_filePath);
            ofs << serialisedConfig;
        }
    }
}