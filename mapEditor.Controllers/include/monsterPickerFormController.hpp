#pragma once

#include "monster.hpp"
#include "types.hpp"
#include <memory>
#include <qicon.h>
#include <string>
#include <map>
#include <vector>

namespace mapeditor::controllers {

class MonsterPickerFormController
{
public:
    MonsterPickerFormController(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores,
                                const std::string &resourcesPath);
    std::vector<std::string> getMonsterStoreNames() const;
    std::vector<std::string> getMonsterIds(const std::string &storeName,
                                           const std::string &filter);
    std::vector<thewarrior::models::MonsterCreationInfo> getMonsters(const std::string &storeName,
                                                                     const std::vector<std::string> &monsterIds) const;
    std::map<std::string, QIcon> getMonstersIcon(const std::string &storeName,
                                                 const std::vector<std::string> &monsterIds);
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores;
    std::string m_resourcesPath;
};

} // namespace mapeditor::controllers
