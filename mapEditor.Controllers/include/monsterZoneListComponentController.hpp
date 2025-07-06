#pragma once

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "glComponentController.hpp"
#include "monsterZoneDTO.hpp"

namespace mapeditor::controllers {

class MonsterZoneListComponentController {
 public:
    explicit MonsterZoneListComponentController(GLComponentController *glComponentController);
    const std::string &getLastError() const;
    std::vector<MonsterZoneDTO> getMonsterZones() const;
    OptMonsterZoneDTOConst getMonsterZoneByName(const std::string &name) const;
    bool isMonsterZonesEmpty() const;
    std::vector<std::string> getAlreadyUsedMonsterZoneNames() const;
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError = "";
};

}  // namespace mapeditor::controllers

