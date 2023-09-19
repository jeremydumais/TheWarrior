#include "monsterZoneDTOUtils.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>
#include "monsterZoneMonsterEncounter.hpp"
#include "rgbItemColor.hpp"

using thewarrior::models::RGBItemColor;
using thewarrior::models::MonsterEncounterRatio;
using thewarrior::models::MonsterZone;
using thewarrior::models::MonsterZoneMonsterEncounter;

namespace mapeditor::controllers {

MonsterZoneDTO MonsterZoneDTOUtils::fromMonsterZone(const thewarrior::models::MonsterZone &zone) {
    std::vector<std::pair<std::string, std::string>> encounters {};
    std::transform(zone.getMonsterEncounters().begin(),
            zone.getMonsterEncounters().end(),
            std::back_inserter(encounters),
            MonsterZoneDTOUtils::fromMonsterZoneMonsterEncounter);
    return MonsterZoneDTO {
        zone.getName(),
        zone.getColor().getName(),
        zone.getColor().getValue(),
        zone.getRatioEncounter(),
        zone.getRatioEncounterOn(),
        encounters
    };
}

std::pair<std::string, std::string> MonsterZoneDTOUtils::fromMonsterZoneMonsterEncounter(const MonsterZoneMonsterEncounter encounter) {
    const std::string ratioStr = [encounter]() {
        switch (encounter.getEncounterRatio()) {
            case MonsterEncounterRatio::LessThanNormal:
                return "Less than normal";
            case MonsterEncounterRatio::Normal:
                return "Normal";
            case MonsterEncounterRatio::Rare:
                return "Rare";
        }
        throw std::runtime_error("unknown monster encouter ratio");
    }();

    return std::pair<std::string, std::string>{ encounter.getMonsterId(), ratioStr };
}

thewarrior::models::MonsterZone MonsterZoneDTOUtils::toMonsterZone(const MonsterZoneDTO &dto) {
    std::vector<MonsterZoneMonsterEncounter> encounters {};
    std::transform(dto.m_monsterEncounters.begin(),
            dto.m_monsterEncounters.end(),
            std::back_inserter(encounters),
            MonsterZoneDTOUtils::toMonsterZoneMonsterEncounter);

    return MonsterZone(dto.m_name,
                       RGBItemColor(dto.m_colorName, dto.m_colorValue),
                       dto.m_ratioEncounter,
                       dto.m_ratioEncounterOn,
                       encounters);
}

MonsterZoneMonsterEncounter MonsterZoneDTOUtils::toMonsterZoneMonsterEncounter(const std::pair<std::string, std::string> &dto) {
    const MonsterEncounterRatio ratio = [dto]() {
        if (dto.second == "Less than normal") {
            return MonsterEncounterRatio::LessThanNormal;
        } else if (dto.second == "Normal") {
            return MonsterEncounterRatio::Normal;
        } else if (dto.second == "Rare") {
            return MonsterEncounterRatio::Rare;
        }
        throw std::invalid_argument("unknown monster encouter ratio");
    }();

    return MonsterZoneMonsterEncounter(dto.first, ratio);
}

}  // namespace mapeditor::controllers
