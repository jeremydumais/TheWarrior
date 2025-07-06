#pragma once

#include <memory>
#include <string>
#include <vector>
#include "iJSONFileStream.hpp"
#include <boost/property_tree/ptree.hpp>

namespace thewarrior::storage {

class ConfigurationManager {
 public:
    explicit ConfigurationManager(const std::string &fileName,
                                  std::unique_ptr<IJSONFileStream> jfs = nullptr);
    const std::string &getLastError() const;
    std::vector<std::string> getVectorOfStringValue(const std::string &path) const;
    std::string getStringValue(const std::string &path) const;
    bool getBoolValue(const std::string &path, bool defaultValue = false) const;
    boost::property_tree::ptree getPTreeNode(const std::string &path) const;
    void setStringValue(const std::string &path, const std::string &value);
    void setBoolValue(const std::string &path, bool value);
    void setVectorOfStringValue(const std::string &path, const std::vector<std::string> &values);
    void setPTreeNode(const std::string &path, const boost::property_tree::ptree &node);
    bool fileExists() const;
    bool load();
    bool save();

 private:
    std::string m_lastError;
    boost::property_tree::ptree m_config;
    std::unique_ptr<IJSONFileStream> m_jfs;
};

}  // namespace thewarrior::storage
