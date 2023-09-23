#pragma once

#include "monsterStore.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "qicon.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace commoneditor::ui {

class MonsterUtils
{
public:
    static std::map<std::string, QIcon> getIconsFromMonsterIds(const std::vector<std::string> &monsterIds,
                                                               const std::shared_ptr<thewarrior::models::MonsterStore> monsterStore,
                                                               const std::string &resourcesPath);
    static std::string getEncounterRatioStr(thewarrior::models::MonsterEncounterRatio ratio);
    static thewarrior::models::MonsterEncounterRatio getEncounterRatioFromString(const std::string &ratioStr);
};

} // namespace commoneditor::ui
