#include "monsterZoneListComponentController.hpp"

namespace mapeditor::controllers {

MonsterZoneListComponentController::MonsterZoneListComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

const std::string &MonsterZoneListComponentController::getLastError() const {
    return m_lastError;
}

std::vector<MonsterZoneDTO> MonsterZoneListComponentController::getMonsterZones() const {
    return m_glComponentController->getMonsterZones();
}

OptMonsterZoneDTOConst MonsterZoneListComponentController::getMonsterZoneByName(const std::string &name) const {
    return m_glComponentController->getMonsterZoneByName(name);
}

bool MonsterZoneListComponentController::isMonsterZonesEmpty() const {
    return m_glComponentController->getMonsterZones().empty();
}

std::vector<std::string> MonsterZoneListComponentController::getAlreadyUsedMonsterZoneNames() const {
    return m_glComponentController->getAlreadyUsedMonsterZoneNames();
}

}  // namespace mapeditor::controllers

