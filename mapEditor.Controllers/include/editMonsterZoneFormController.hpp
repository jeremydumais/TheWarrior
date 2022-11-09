#pragma once

#include "monsterStoreInfo.hpp"
#include "monsterEncounterDTO.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace mapeditor::controllers {

class EditMonsterZoneFormController
{
public:
    EditMonsterZoneFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                                  const std::string resourcesPath);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores();
    std::vector<MonsterEncounterDTO> getMonsterEncounters() const;
    std::vector<std::string> getMonsterEncounterIds() const;
    static std::string getEncounterRatioStr(thewarrior::models::MonsterEncounterRatio ratio);
    static thewarrior::models::MonsterEncounterRatio getEncounterRatioFromString(const std::string &ratioStr);
    bool addMonsterEncounter(mapeditor::controllers::MonsterEncounterDTO monsterEncounter);
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
    std::string m_resourcesPath;
    std::string m_lastError = "";
    std::string m_emptyName = "";
    std::vector<thewarrior::models::MonsterZoneMonsterEncounter> m_monsterEncounters = {};
    std::string getMonsterNameById(const std::string &monsterId) const;
    std::map<std::string, QIcon> getMonsterIconByMonsterIds(const std::vector<std::string> &monsterIds) const;
};

} // namespace mapeditor::controllers
