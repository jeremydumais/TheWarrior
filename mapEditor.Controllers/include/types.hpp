#pragma once

#include "monsterStore.hpp"
#include <memory>
#include <unordered_map>

namespace mapeditor::controllers {

typedef std::unordered_map<std::string, std::shared_ptr<thewarrior::models::MonsterStore>> ContainerOfMonsterStore;

} // namespace mapeditor::controllers

