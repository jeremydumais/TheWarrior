#pragma once

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class ConfigurationManager {
public:
    explicit ConfigurationManager(const std::string &file);
    const std::vector<std::string> getVectorOfStringValue(const std::string &path) const;
    const std::string getStringValue(const std::string &path) const;
    const std::string &getLastError() const;
    void setStringValue(const std::string &path, const std::string &value);
    void setVectorOfStringValue(const std::string &path, const std::vector<std::string> &values);
    bool save();
private:
    std::string configFile;
    boost::property_tree::ptree config;
    std::string lastError;
};