#pragma once

#include <string>
#include <utility>
#include <vector>

namespace mapeditor::controllers {

struct MonsterZoneDTO {
    std::string m_name;
    std::string m_colorName;
    std::string m_colorValue;
    unsigned int m_ratioEncounter;
    unsigned int m_ratioEncounterOn;
    std::vector<std::pair<std::string, std::string>> m_monsterEncounters;
};

}  // namespace mapeditor::controllers

