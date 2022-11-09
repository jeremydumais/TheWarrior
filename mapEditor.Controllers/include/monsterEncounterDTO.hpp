#pragma once

#include <qicon.h>
#include <optional>
#include <string>

namespace mapeditor::controllers {

struct MonsterEncounterDTO
{
    std::string monsterId;
    std::string monsterName;
    std::string encounterRatio;
    std::optional<QIcon> monsterIcon;
};

} // namespace mapeditor::controllers
