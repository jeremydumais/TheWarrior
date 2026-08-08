#include <optional>
#include <string>
#include <vector>
#include "npcListComponentController.hpp"
#include "npcDTO.hpp"

using mapeditor::controllers::NPCDTO;

namespace mapeditor::controllers {

NPCListComponentController::NPCListComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

const GLComponentController *NPCListComponentController::getGLComponentController() const {
    return m_glComponentController;
}

const std::string &NPCListComponentController::getLastError() const {
    return m_lastError;
}

const std::vector<std::string> NPCListComponentController::getAlreadyUsedNPCIds() const {
    return m_glComponentController->getAlreadyUsedNPCIds();
}

const std::vector<NPCDTO> NPCListComponentController::getNPCs() const {
    return m_glComponentController->getNPCs();
}

const std::optional<NPCDTO> NPCListComponentController::getNPCById(const std::string &name) const {
    return m_glComponentController->getNPCById(name);
}

const std::string &NPCListComponentController::getEditedId() const {
    return m_editedId;
}

void NPCListComponentController::setEditedId(const std::string &id) {
    m_editedId = id;
}

}  // namespace mapeditor::controllers

