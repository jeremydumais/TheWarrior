#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>

class ConfigurationManager {
public:
    explicit ConfigurationManager(const std::string &file);
    const std::string getStringValue(const std::string &path) const;
    const std::string &getLastError() const;
    void setStringValue(const std::string &path, const std::string &value);
    bool save();
	static const std::string THEME_PATH;
private:
    std::string configFile;
    boost::property_tree::ptree config;
    std::string lastError;
};