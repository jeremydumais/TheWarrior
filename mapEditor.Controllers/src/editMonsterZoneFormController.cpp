#include "editMonsterZoneFormController.hpp"
#include "monsterStoreStorage.hpp"

using namespace thewarrior::storage;

namespace mapeditor::controllers {

const std::string &EditMonsterZoneFormController::getLastError() const
{
    return m_lastError;
}

} // namespace mapeditor::controllers
