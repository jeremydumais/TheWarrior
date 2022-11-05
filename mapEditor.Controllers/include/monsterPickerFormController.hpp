#pragma once

#include "types.hpp"
#include <memory>

namespace mapeditor::controllers {

class MonsterPickerFormController
{
public:
    MonsterPickerFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores() const;
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
};

} // namespace mapeditor::controllers
