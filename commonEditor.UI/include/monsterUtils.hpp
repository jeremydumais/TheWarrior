#pragma once

#include "monsterStore.hpp"
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
};

} // namespace commoneditor::ui
