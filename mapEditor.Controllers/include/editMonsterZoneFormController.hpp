#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "monster.hpp"
#include "monsterStoreInfo.hpp"
#include "monsterEncounterDTO.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class EditMonsterZoneFormController {
 public:
    EditMonsterZoneFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                                  const std::string &resourcesPath,
                                  const std::optional<MonsterZoneDTO> selectedZone,
                                  const std::vector<std::string> &alreadyUsedZoneNames);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores();
    std::vector<MonsterEncounterDTO> getMonsterEncounters() const;
    std::vector<std::string> getMonsterEncounterIds() const;
    bool isMonsterZoneNameAlreadyUsed(const std::string &zoneName) const;
    bool isAtLeastAMonsterEncounterWithNormalRatio() const;
    bool addMonsterEncounter(mapeditor::controllers::MonsterEncounterDTO monsterEncounter);
    bool updateMonsterEncounter(const std::string &oldMonsterId,
                                mapeditor::controllers::MonsterEncounterDTO monsterEncounter);
    bool removeMonsterEncounter(const std::string &monsterId);

 private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
    std::string m_resourcesPath;
    std::string m_lastError = "";
    std::string m_emptyName = "";
    std::vector<thewarrior::models::MonsterZoneMonsterEncounter> m_monsterEncounters = {};
    std::optional<MonsterZoneDTO> m_selectedZone;
    std::vector<std::string> m_alreadyUsedZoneNames;
    const std::shared_ptr<const thewarrior::models::Monster> getMonsterById(const std::string &monsterId) const;
    std::string getMonsterNameById(const std::string &monsterId) const;
    std::map<std::string, QIcon> getMonsterIconByMonsterIds(const std::vector<std::string> &monsterIds) const;
    std::vector<thewarrior::models::MonsterZoneMonsterEncounter>::iterator getMonsterEncounterById(const std::string &monsterId);
};

}  // namespace mapeditor::controllers
