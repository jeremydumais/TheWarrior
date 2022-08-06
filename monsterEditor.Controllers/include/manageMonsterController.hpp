#pragma once

#include "monsterStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <string>

namespace monstereditor::controllers {

struct MonsterDTO
{
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    int health;
    float attack;
    float defense;
    int goldMinimum;
    int goldMaximum;
    virtual ~MonsterDTO() = default; //Used to make the struct polymorphic
};

class ManageMonsterController
{
public:
    ManageMonsterController(std::shared_ptr<MonsterStore> monsterStore);
    virtual ~ManageMonsterController() = default;
    const std::string &getLastError() const;
    const TextureContainer &getTextureContainer() const;
    virtual std::unique_ptr<MonsterDTO> getMonster(const std::string &id) const;
    bool addMonster(std::unique_ptr<MonsterDTO> monsterInfo);
    bool updateMonster(std::unique_ptr<MonsterDTO> monsterInfo,
                    const std::string &oldMonsterId);
    bool deleteMonster(const std::string &monsterId);
protected:
    std::shared_ptr<MonsterStore> m_monsterStore;
    std::string m_lastError;
    std::shared_ptr<MonsterStore> getMonsterStore();
    virtual std::shared_ptr<Monster> monsterDTOToMonster(std::unique_ptr<MonsterDTO> dto);
};

} // namespace ManageMonsterController
