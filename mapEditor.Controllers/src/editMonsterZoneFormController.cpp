#include "editMonsterZoneFormController.hpp"
#include "editMonsterEncounterFormController.hpp"
#include "monsterEncounterDTO.hpp"
#include "monsterStoreStorage.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "monsterUtils.hpp"
#include "types.h"
#include <fmt/format.h>
#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <qicon.h>
#include <stdexcept>
#include <string>
#include <vector>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;
using namespace thewarrior::models;
using namespace thewarrior::storage;

namespace mapeditor::controllers {

EditMonsterZoneFormController::EditMonsterZoneFormController(const std::shared_ptr<ContainerOfMonsterStore> monsterStores,
                                                             const std::string resourcesPath)
    : m_monsterStores(monsterStores),
      m_resourcesPath(resourcesPath)
{
}

std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> EditMonsterZoneFormController::getMonsterStores()
{
    return m_monsterStores;
}

const std::string &EditMonsterZoneFormController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &EditMonsterZoneFormController::getLastError() const
{
    return m_lastError;
}

std::vector<MonsterEncounterDTO> EditMonsterZoneFormController::getMonsterEncounters() const
{
    std::vector<MonsterEncounterDTO> retval = {};
    std::vector<std::string> monsterIds = getMonsterEncounterIds();
    std::map<std::string, QIcon> icons = getMonsterIconByMonsterIds(monsterIds);
    std::transform(m_monsterEncounters.begin(),
                   m_monsterEncounters.end(),
                   std::back_inserter(retval),
                   [this, &icons](const auto &monsterEncounter) {
                        std::optional<QIcon> icon = icons.contains(monsterEncounter.getMonsterId()) ?
                                                    std::make_optional(icons.at(monsterEncounter.getMonsterId())) :
                                                    std::nullopt;
                        return MonsterEncounterDTO {
                            .monsterId = monsterEncounter.getMonsterId(),
                            .monsterName = getMonsterNameById(monsterEncounter.getMonsterId()),
                            .encounterRatio = MonsterUtils::getEncounterRatioStr(monsterEncounter.getEncounterRatio()),
                            .monsterIcon = icon
                        };
                   });
    return retval;
}

std::vector<std::string> EditMonsterZoneFormController::getMonsterEncounterIds() const
{
    std::vector<std::string> monsterIds = {};
    std::transform(m_monsterEncounters.begin(),
                   m_monsterEncounters.end(),
                   std::back_inserter(monsterIds),
                   [](const auto &monster) {
                        return monster.getMonsterId();
                   });
    return monsterIds;
}

std::string EditMonsterZoneFormController::getMonsterNameById(const std::string &monsterId) const
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

std::map<std::string, QIcon> EditMonsterZoneFormController::getMonsterIconByMonsterIds(const std::vector<std::string> &monsterIds) const
{
    std::map<std::string, QIcon> icons = {};
    if (m_monsterStores != nullptr) {
        for (const auto &store : *m_monsterStores) {
            for (const auto &item : MonsterUtils::getIconsFromMonsterIds(monsterIds,
                                                              store.second,
                                                              m_resourcesPath)) {
                icons.insert(item);
            }
        }
    }
    return icons;
}

bool EditMonsterZoneFormController::addMonsterEncounter(mapeditor::controllers::MonsterEncounterDTO monsterEncounter)
{
    try {
        MonsterEncounterRatio ratio = [monsterEncounter]() {
            return MonsterUtils::getEncounterRatioFromString(monsterEncounter.encounterRatio);
        }();
        m_monsterEncounters.push_back(MonsterZoneMonsterEncounter(monsterEncounter.monsterId, ratio));
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }

    return false;
}

bool EditMonsterZoneFormController::updateMonsterEncounter(const std::string &oldMonsterId,
                                                           MonsterEncounterDTO monsterEncounter)
{
    //TODO Complete this method
    return false;
}

}// namespace mapeditor::controllers
