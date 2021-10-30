#include "configurationManager.hpp"
#include "jsonFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fmt/format.h>

using namespace std;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::algorithm;

ConfigurationManager::ConfigurationManager(const string &fileName,
                                           unique_ptr<IJSONFileStream> jfs)
    : lastError(""),
      config(),
      jfs(nullptr)
{
    if (trim_copy(fileName).empty())
    {
        throw invalid_argument("The filename cannot be empty!");
    }

    if (jfs == nullptr)
    {
        this->jfs = make_unique<JSONFileStream>(fileName);
    }
    else
    {
        this->jfs = move(jfs);
    }
}

const std::string &ConfigurationManager::getLastError() const
{
    return lastError;
}

std::vector<std::string> ConfigurationManager::getVectorOfStringValue(const std::string &path) const
{
    auto retVal = vector<string>();
    if (config.get_child_optional(path).has_value())
    {
        for (const auto &item : config.get_child(path))
        {
            retVal.push_back(item.second.data());
        }
    }
    return retVal;
}

std::string ConfigurationManager::getStringValue(const string &path) const
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

    for (const auto &item : values)
    {
        ptree recent_node;
        recent_node.put("", item);
        recents_node.push_back(make_pair("", recent_node));
    }
    config.put_child(path, recents_node);
}

bool ConfigurationManager::load()
{
    if (jfs->fileExists())
    {
        if (!jfs->readFile(config))
        {
            this->lastError = jfs->getLastError();
            return false;
        }
    }
    else
    {
        this->lastError = fmt::format("The file {0} doesn't exist.", jfs->getFileName());
        return false;
    }
    return true;
}

bool ConfigurationManager::save()
{
    if (!jfs->writeFile(config))
    {
        this->lastError = jfs->getLastError();
        return false;
    }
    return true;
}
