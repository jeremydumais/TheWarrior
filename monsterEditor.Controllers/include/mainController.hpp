#pragma once

#include "monsterStore.hpp"
#include "textureContainer.hpp"
#include <qicon.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace monstereditor::controllers {

struct MonsterListDisplay
{
    std::string id;
    std::string name;
    int health;
    float attack;
    float defense;
    int goldMinimum;
    int goldMaximum;
};

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
    std::shared_ptr<MonsterStore> getMonsterStore();
    const std::string &getLastError() const;
    bool openMonsterStore(const std::string &fileName);
    bool saveMonsterStore(const std::string &fileName);
    std::vector<MonsterListDisplay> getMonsters() const;
    std::map<std::string, QIcon> getIconsFromMonsterIds(std::vector<std::string> monsterIds,
                                                     const std::string &resourcesPath) const;
    bool deleteMonster(const std::string &id);
private:
    std::shared_ptr<MonsterStore> m_monsterStore;
    std::string m_lastError;
};

} //namespace monstereditor::controllers
