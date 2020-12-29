#include "configurationManager.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::property_tree;

const std::string ConfigurationManager::THEME_PATH { "Display.Theme" };

ConfigurationManager::ConfigurationManager(const string &file)
    : configFile(file)
{
    if (exists(file)) {
        read_json(file, config);
    }    
}

const std::string ConfigurationManager::getStringValue(const std::string &path) const
{
    return config.get<string>(path, "");
}

const std::string& ConfigurationManager::getLastError() const
{
    return lastError;
}

void ConfigurationManager::setStringValue(const std::string &path, const std::string &value) 
{
    config.put(path, value);
}

bool ConfigurationManager::save() 
{
    try {
        write_json(configFile, config);
        return true;
    }
    catch(json_parser_error &err) {
        lastError = err.what();
        return false;
    }
}



