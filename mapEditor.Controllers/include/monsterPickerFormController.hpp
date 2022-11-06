#pragma once

#include "monster.hpp"
#include "types.hpp"
#include <memory>

namespace mapeditor::controllers {

class MonsterPickerFormController
{
public:
    MonsterPickerFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    std::vector<std::string> getMonsterStoreNames() const;
    std::vector<thewarrior::models::MonsterCreationInfo> getMonsters(const std::string &storeName,
                                                                     const std::string &filter) const;
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
};

} // namespace mapeditor::controllers
