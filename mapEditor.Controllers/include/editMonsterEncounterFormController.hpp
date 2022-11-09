#pragma once

#include "types.hpp"
#include <memory>
#include <optional>
#include <qicon.h>
#include <vector>

namespace mapeditor::controllers {

class EditMonsterEncounterFormController
{
public:
    EditMonsterEncounterFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                                       const std::string &resourcesPath,
                                       const std::vector<std::string> &alreadyUsedMonsterIds);
    const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores() const;
    const std::string &getResourcesPath() const;
    const std::string &getMonsterNameById(const std::string &monsterId) const;
    std::optional<QIcon> getMonsterIcon(const std::string &monsterId) const;
    bool isMonsterIdValid(const std::string &monsterId) const;
    bool isMonsterAlreadyUsed(const std::string &monsterId) const;
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
    std::string m_resourcesPath;
    std::vector<std::string> m_alreadyUsedMonsterIds;
    std::string m_emptyName = "";
};

} // namespace mapeditor::controllers
