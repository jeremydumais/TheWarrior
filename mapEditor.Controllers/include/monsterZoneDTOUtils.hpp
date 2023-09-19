#pragma once

#include <string>
#include <utility>
#include "monsterZoneDTO.hpp"
#include "monsterZone.hpp"
#include "monsterZoneMonsterEncounter.hpp"

namespace mapeditor::controllers {

class MonsterZoneDTOUtils {
 public:
    static MonsterZoneDTO fromMonsterZone(const thewarrior::models::MonsterZone &zone);
    static std::pair<std::string, std::string> fromMonsterZoneMonsterEncounter(const thewarrior::models::MonsterZoneMonsterEncounter encounter);
    static thewarrior::models::MonsterZone toMonsterZone(const MonsterZoneDTO &dto);
    static thewarrior::models::MonsterZoneMonsterEncounter toMonsterZoneMonsterEncounter(const std::pair<std::string, std::string> &dto);
};

}  // namespace mapeditor::controllers

