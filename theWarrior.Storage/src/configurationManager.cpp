#include "configurationManager.hpp"
#include "jsonFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>

using namespace std;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::algorithm;

ConfigurationManager::ConfigurationManager(const string &fileName,
                                           unique_ptr<IJSONFileStream> jfs)
{
    if (trim_copy(fileName).empty()) {
        throw invalid_argument("The filename cannot be empty!");
    }    

    if (jfs == nullptr) {
        this->jfs = make_unique<JSONFileStream>(fileName);
    }  
}

const std::string& ConfigurationManager::getLastError() const
{
    return lastError;
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

bool ConfigurationManager::load() 
{
    if (jfs->fileExists()) {
        if (!jfs->readFile(config)) {
            lastError = jfs->getLastError();
            return false;
        }
    }
    else {
        lastError = fmt::format("The file {0} doesn't exist.", jfs->getFileName());
        return false;
    }
    return true;
}

bool ConfigurationManager::save() 
{
    /*try {
        write_json(configFile, config);
        return true;
    }
    catch(json_parser_error &err) {
        this->setLastError(err.what());
        return false;
    }*/
    return false;
}



