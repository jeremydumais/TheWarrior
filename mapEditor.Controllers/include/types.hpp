#pragma once

#include "monsterStore.hpp"
#include <memory>
#include <vector>

namespace mapeditor::controllers {

typedef std::vector<std::shared_ptr<thewarrior::models::MonsterStore>> VecOfMonsterStore;

} // namespace mapeditor::controllers

