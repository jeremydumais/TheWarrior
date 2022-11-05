#include "monsterPickerFormController.hpp"
#include "types.hpp"
#include <memory>

namespace mapeditor::controllers {

MonsterPickerFormController::MonsterPickerFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : m_monsterStores(monsterStores)
{
}

const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> MonsterPickerFormController::getMonsterStores() const
{
    return m_monsterStores;
}

} // namespace mapeditor::controllers
