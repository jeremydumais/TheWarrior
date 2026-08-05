#pragma once

#include "itemStore.hpp"
#include "monsterStore.hpp"
#include <memory>
#include <unordered_map>

namespace mapeditor::controllers
{
    using ContainerOfMonsterStore = std::unordered_map<std::string, std::shared_ptr<thewarrior::models::MonsterStore>>;
    using ContainerOfItemStore = std::unordered_map<std::string, std::shared_ptr<thewarrior::models::ItemStore>>;

} // namespace mapeditor::controllers
