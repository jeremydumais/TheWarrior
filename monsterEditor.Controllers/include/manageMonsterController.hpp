#pragma once

#include <memory>
#include <string>
#include <utility>
#include "monsterStore.hpp"
#include "textureContainer.hpp"

namespace monstereditor::controllers {

struct MonsterDTO {
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    std::pair<int, int> healthRange;
    int maxHealth;
    float attack;
    float defense;
    std::pair<int, int> gold;
    std::pair<int, int> experience;
    thewarrior::models::MonsterType type = thewarrior::models::MonsterType::Regular;
    std::string musicFilename;
    virtual ~MonsterDTO() = default;  // Used to make the struct polymorphic
};

class ManageMonsterController {
 public:
    explicit ManageMonsterController(std::shared_ptr<thewarrior::models::MonsterStore> monsterStore);
    virtual ~ManageMonsterController() = default;
    const std::string &getLastError() const;
    const thewarrior::models::TextureContainer &getTextureContainer() const;
    virtual std::unique_ptr<MonsterDTO> getMonster(const std::string &id) const;
    bool addMonster(std::unique_ptr<MonsterDTO> monsterInfo);
    bool updateMonster(std::unique_ptr<MonsterDTO> monsterInfo,
                    const std::string &oldMonsterId);
    bool deleteMonster(const std::string &monsterId);

 protected:
    std::shared_ptr<thewarrior::models::MonsterStore> m_monsterStore;
    std::string m_lastError;
    std::shared_ptr<thewarrior::models::MonsterStore> getMonsterStore();
    virtual std::shared_ptr<thewarrior::models::Monster> monsterDTOToMonster(std::unique_ptr<MonsterDTO> dto);
};

}  // namespace monstereditor::controllers
