#include "editMonsterEncounterFormController.hpp"
#include "monster.hpp"
#include "monsterUtils.hpp"
#include "types.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <exception>
#include <memory>
#include <optional>

using namespace commoneditor::ui;
using namespace thewarrior::models;
using namespace boost::algorithm;

namespace mapeditor::controllers {

EditMonsterEncounterFormController::EditMonsterEncounterFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores,
                                                                       const std::string &resourcesPath,
                                                                       const std::vector<std::string> &alreadyUsedMonsterIds)
    : m_monsterStores(monsterStores),
      m_resourcesPath(resourcesPath),
      m_alreadyUsedMonsterIds(alreadyUsedMonsterIds)
{
}

const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> EditMonsterEncounterFormController::getMonsterStores() const
{
    return m_monsterStores;
}

const std::string &EditMonsterEncounterFormController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &EditMonsterEncounterFormController::getMonsterNameById(const std::string &monsterId) const
{
    if (m_monsterStores != nullptr) {
        for (const auto &store : *m_monsterStores) {
            const auto monster = store.second->findMonster(monsterId);
            if (monster != nullptr) {
                return monster->getName();
            }
        }
    }
    return m_emptyName;
}

std::optional<QIcon> EditMonsterEncounterFormController::getMonsterIcon(const std::string &monsterId) const
{
    if (m_monsterStores != nullptr) {
        for (const auto &store : *m_monsterStores) {
            const auto icon = MonsterUtils::getIconsFromMonsterIds({ monsterId },
                                            store.second,
                                            m_resourcesPath);
            if (icon.contains(monsterId)) {
                return icon.at(monsterId);
            }
        }
    }
    return std::nullopt;
}

bool EditMonsterEncounterFormController::isMonsterIdValid(const std::string &monsterId) const
{
    try {
        Monster::validateId(monsterId);
        return true;
    }
    catch(const std::exception &) {
        return false;
    }
}

bool EditMonsterEncounterFormController::isMonsterAlreadyUsed(const std::string &monsterId) const
{
    return std::find_if(m_alreadyUsedMonsterIds.begin(),
                        m_alreadyUsedMonsterIds.end(),
                        [monsterId](const std::string &id) {
                            return to_upper_copy(monsterId) == to_upper_copy(id);
                        }) != m_alreadyUsedMonsterIds.end();
}

} // namespace mapeditor::controllers
