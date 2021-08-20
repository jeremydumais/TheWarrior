#include "configurationManager.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::property_tree;

const std::string ConfigurationManager::THEME_PATH { "Display.Theme" };
const std::string ConfigurationManager::RECENT_MAPS { "ItemsDB.Recents" };

ConfigurationManager::ConfigurationManager(const string &file)
    : configFile(file)
{
    if (exists(file)) {
        read_json(file, config);
    }    
}

const std::vector<std::string> ConfigurationManager::getVectorOfStringValue(const std::string &path) const
{
    vector<string> retVal;
    if (config.get_child_optional(path).has_value()) {
        for (const auto &item : config.get_child(path)) {
            retVal.push_back(item.second.data());
        }
    }
    return retVal;
}

const std::string ConfigurationManager::getStringValue(const string &path) const
{
    return config.get<string>(path, "");
}

const std::string& ConfigurationManager::getLastError() const
{
    return lastError;
}

void ConfigurationManager::setStringValue(const string &path, const string &value) 
{
    config.put(path, value);
}

void ConfigurationManager::setVectorOfStringValue(const string &path, const vector<string> &values) 
{
    ptree recents_node;

    for(const auto &item : values) {
        ptree recent_node;
        recent_node.put("", item);
        recents_node.push_back(make_pair("", recent_node));
    }
    config.put_child(path,recents_node);
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



