#include <string>
#include "npcListComponentController.hpp"

namespace mapeditor::controllers {

NPCListComponentController::NPCListComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

const std::string &NPCListComponentController::getLastError() const {
    return m_lastError;
}

}  // namespace mapeditor::controllers

