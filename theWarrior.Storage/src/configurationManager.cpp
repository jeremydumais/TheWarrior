#include "configurationManager.hpp"
#include <fmt/format.h>
#include <sys/ucontext.h>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/algorithm/string/trim.hpp"
#include "jsonFileStream.hpp"

using boost::property_tree::ptree;
using boost::algorithm::trim_copy;

namespace thewarrior::storage {

ConfigurationManager::ConfigurationManager(const std::string &fileName,
                                           std::unique_ptr<IJSONFileStream> jfs)
    : m_lastError(""),
      m_config(),
      m_jfs(nullptr) {
    if (trim_copy(fileName).empty()) {
        throw std::invalid_argument("The filename cannot be empty!");
    }

    if (jfs == nullptr) {
        this->m_jfs = make_unique<JSONFileStream>(fileName);
    } else {
        this->m_jfs = std::move(jfs);
    }
}

const std::string &ConfigurationManager::getLastError() const {
    return m_lastError;
}

std::vector<std::string> ConfigurationManager::getVectorOfStringValue(const std::string &path) const {
    auto retVal = std::vector<std::string>();
    if (m_config.get_child_optional(path).has_value()) {
        for (const auto &item : m_config.get_child(path)) {
            retVal.push_back(item.second.data());
        }
    }
    return retVal;
}

std::string ConfigurationManager::getStringValue(const std::string &path) const {
    return m_config.get<std::string>(path, "");
}

bool ConfigurationManager::getBoolValue(const std::string &path, bool defaultValue) const {
    return m_config.get<bool>(path, defaultValue);
}

boost::property_tree::ptree ConfigurationManager::getPTreeNode(const std::string &path) const {
    return m_config.get_child(path, ptree());
}

void ConfigurationManager::setStringValue(const std::string &path, const std::string &value) {
    m_config.put<std::string>(path, value);
}

void ConfigurationManager::setBoolValue(const std::string &path, bool value) {
    m_config.put<bool>(path, value);
}

void ConfigurationManager::setVectorOfStringValue(const std::string &path,
        const std::vector<std::string> &values) {
    ptree recents_node;

    for (const auto &item : values) {
        ptree recent_node;
        recent_node.put("", item);
        recents_node.push_back(make_pair("", recent_node));
    }
    m_config.put_child(path, recents_node);
}

void ConfigurationManager::setPTreeNode(const std::string &path,
        const boost::property_tree::ptree &node) {
    m_config.put_child(path, node);
}

bool ConfigurationManager::fileExists() const {
    return m_jfs->fileExists();
}

bool ConfigurationManager::load() {
    if (m_jfs->fileExists()) {
        if (!m_jfs->readFile(m_config)) {
            this->m_lastError = m_jfs->getLastError();
            return false;
        }
    } else {
        this->m_lastError = fmt::format("The file {0} doesn't exist.", m_jfs->getFileName());
        return false;
    }
    return true;
}

bool ConfigurationManager::save() {
    if (!m_jfs->writeFile(m_config)) {
        this->m_lastError = m_jfs->getLastError();
        return false;
    }
    return true;
}

}  // namespace thewarrior::storage
