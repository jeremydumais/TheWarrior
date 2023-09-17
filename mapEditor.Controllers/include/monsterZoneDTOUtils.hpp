#pragma once

#include "monsterZoneDTO.hpp"
#include "monsterZone.hpp"

namespace mapeditor::controllers {

class MonsterZoneDTOUtils {
 public:
    static MonsterZoneDTO fromMonsterZone(const thewarrior::models::MonsterZone &zone);
    static thewarrior::models::MonsterZone toMonsterZone(const MonsterZoneDTO &dto);
};

}  // namespace mapeditor::controllers

