#include <string>
#include <vector>
#include "npcListComponentController.hpp"
#include "npcDTO.hpp"

using mapeditor::controllers::NPCDTO;

namespace mapeditor::controllers {

NPCListComponentController::NPCListComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
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

}  // namespace mapeditor::controllers

