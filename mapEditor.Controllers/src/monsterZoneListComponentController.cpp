#include "monsterZoneListComponentController.hpp"

namespace mapeditor::controllers {

MonsterZoneListComponentController::MonsterZoneListComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

const std::string &MonsterZoneListComponentController::getLastError() const {
    return m_lastError;
}

}  // namespace mapeditor::controllers

