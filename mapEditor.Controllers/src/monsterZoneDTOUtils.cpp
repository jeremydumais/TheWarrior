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
            [](const MonsterZoneMonsterEncounter &encounter) {
                const std::string ratioStr = [encounter]() {
                    switch (encounter.getEncounterRatio()) {
                        case MonsterEncounterRatio::LessThanNormal:
                            return "Less than normal";
                        break;
                        case MonsterEncounterRatio::Normal:
                            return "Normal";
                        break;
                        case MonsterEncounterRatio::Rare:
                            return "Rare";
                        break;
                        default:
                            throw std::runtime_error("unknown monster encouter ratio");
                    }
                }();

                return std::pair<std::string, std::string>{ encounter.getMonsterId(), ratioStr };
            });
    return MonsterZoneDTO {
        zone.getName(),
        zone.getColor().getName(),
        zone.getColor().getValue(),
        zone.getRatioEncounter(),
        zone.getRatioEncounterOn(),
        encounters
    };
}

thewarrior::models::MonsterZone MonsterZoneDTOUtils::toMonsterZone(const MonsterZoneDTO &dto) {
    std::vector<MonsterZoneMonsterEncounter> encounters {};
    std::transform(dto.m_monsterEncounters.begin(),
            dto.m_monsterEncounters.end(),
            std::back_inserter(encounters),
            [](const std::pair<std::string, std::string> &encounter) {
                const MonsterEncounterRatio ratio = [encounter]() {
                    if (encounter.second == "Less than normal") {
                        return MonsterEncounterRatio::LessThanNormal;
                    } else if (encounter.second == "Normal") {
                        return MonsterEncounterRatio::Normal;
                    } else if (encounter.second == "Rare") {
                        return MonsterEncounterRatio::Rare;
                    }
                throw std::invalid_argument("unknown monster encouter ratio");
                }();

                return MonsterZoneMonsterEncounter(encounter.first, ratio);
            });

    return MonsterZone(dto.m_name,
                       RGBItemColor(dto.m_colorName, dto.m_colorValue),
                       dto.m_ratioEncounter,
                       dto.m_ratioEncounterOn,
                       encounters);
}

}  // namespace mapeditor::controllers
