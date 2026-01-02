#include "editNPCFormController.hpp"
#include <string>

namespace mapeditor::controllers {

EditNPCFormController::EditNPCFormController(const std::string &resourcesPath)
    : m_resourcesPath(resourcesPath) {
}

const std::string &EditNPCFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditNPCFormController::getLastError() const {
    return m_lastError;
}

}  // namespace mapeditor::controllers
