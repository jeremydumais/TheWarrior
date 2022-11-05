#include "editMonsterEncounterFormController.hpp"
#include "types.hpp"
#include <memory>

namespace mapeditor::controllers {

EditMonsterEncounterFormController::EditMonsterEncounterFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : m_monsterStores(monsterStores)
{
}

const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> EditMonsterEncounterFormController::getMonsterStores() const
{
    return m_monsterStores;
}

} // namespace mapeditor::controllers
