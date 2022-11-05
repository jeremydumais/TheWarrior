#include "editMonsterZoneFormController.hpp"
#include "monsterStoreStorage.hpp"
#include "types.h"
#include <memory>

using namespace mapeditor::controllers;
using namespace thewarrior::storage;

namespace mapeditor::controllers {

EditMonsterZoneFormController::EditMonsterZoneFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : m_monsterStores(monsterStores)
{
}

std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> EditMonsterZoneFormController::getMonsterStores()
{
    return m_monsterStores;
}

const std::string &EditMonsterZoneFormController::getLastError() const
{
    return m_lastError;
}

} // namespace mapeditor::controllers
