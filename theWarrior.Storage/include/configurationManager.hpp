#pragma once

#include "iJSONFileStream.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>
#include <vector>

class ConfigurationManager
{
public:
    explicit ConfigurationManager(const std::string &fileName,
                                  std::unique_ptr<IJSONFileStream> jfs = nullptr);
    const std::string &getLastError() const;
    std::vector<std::string> getVectorOfStringValue(const std::string &path) const;
    std::string getStringValue(const std::string &path) const;
    void setStringValue(const std::string &path, const std::string &value);
    void setVectorOfStringValue(const std::string &path, const std::vector<std::string> &values);
    bool load();
    bool save();
private:
    std::string m_lastError;
    boost::property_tree::ptree m_config;
    std::unique_ptr<IJSONFileStream> m_jfs;
};