#pragma once

#include <string>

namespace mapeditor::controllers {

class EditNPCFormController {
 public:
    explicit EditNPCFormController(const std::string &resourcesPath);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;

 private:
    std::string m_resourcesPath;
    std::string m_lastError = "";
};

}  // namespace mapeditor::controllers
