#include <fmt/format.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "manageMonsterController.hpp"

using namespace thewarrior::models;

namespace monstereditor::controllers {

ManageMonsterController::ManageMonsterController(std::shared_ptr<MonsterStore> monsterStore)
    : m_monsterStore(monsterStore) {
}

const std::string& ManageMonsterController::getLastError() const {
    return m_lastError;
}

std::shared_ptr<MonsterStore> ManageMonsterController::getMonsterStore() {
    return m_monsterStore;
}

const TextureContainer &ManageMonsterController::getTextureContainer() const {
    return m_monsterStore->getTextureContainer();
}

std::unique_ptr<MonsterDTO> ManageMonsterController::getMonster(const std::string &id) const {
    auto monster = m_monsterStore->findMonster(id);
    if (monster != nullptr) {
        auto retval = std::make_unique<MonsterDTO>();
        retval->id = monster->getId();
        retval->name = monster->getName();
        retval->textureName = monster->getTextureName();
        retval->textureIndex = monster->getTextureIndex();
        retval->healthRange = monster->getHealthRange();
        retval->maxHealth = monster->getMaxHealth();
        retval->attack = monster->getAttack();
        retval->defense = monster->getDefense();
        retval->gold = monster->getGoldRewardRange();
        retval->experience = monster->getExperienceRewardRange();
        retval->type = monster->getType();
        retval->musicFilename = monster->getMusicFilename();
        return retval;
    }
    return nullptr;
}

bool ManageMonsterController::addMonster(std::unique_ptr<MonsterDTO> monsterInfo) {
    std::shared_ptr<Monster> newMonster = monsterDTOToMonster(std::move(monsterInfo));
    if (newMonster == nullptr) {
        return false;
    }
    if (m_monsterStore->isMonsterExists(newMonster->getId())) {
        m_lastError = fmt::format("Monster {0} already exist in the store.", newMonster->getId());
        return false;
    }
    if (!m_monsterStore->addMonster(newMonster)) {
        m_lastError = m_monsterStore->getLastError();
        return false;
    }
    return true;
}

bool ManageMonsterController::updateMonster(std::unique_ptr<MonsterDTO> monsterInfo,
                                      const std::string &oldMonsterId) {
    if (monsterInfo == nullptr) {
        m_lastError = "No monsterInfo structure has been provided.";
        return false;
    }
    std::shared_ptr<Monster> updateMonster = monsterDTOToMonster(std::move(monsterInfo));
    if (updateMonster == nullptr) {
        return false;
    }
    if (updateMonster->getId() != oldMonsterId && m_monsterStore->isMonsterExists(updateMonster->getId())) {
        m_lastError = fmt::format("Monster {0} already exist in the store.", updateMonster->getId());
        return false;
    }
    if (!m_monsterStore->replaceMonster(oldMonsterId, updateMonster)) {
        m_lastError = m_monsterStore->getLastError();
        return false;
    }
    return true;
}

bool ManageMonsterController::deleteMonster(const std::string &monsterId) {
    if (!m_monsterStore->removeMonster(monsterId)) {
        m_lastError = m_monsterStore->getLastError();
        return false;
    }
    return true;
}

std::shared_ptr<Monster> ManageMonsterController::monsterDTOToMonster(std::unique_ptr<MonsterDTO> dto) {
    MonsterCreationInfo creationInfo = {
        .id = dto->id,
        .name = dto->name,
        .textureName = dto->textureName,
        .textureIndex = dto->textureIndex,
        .healthRange = dto->healthRange,
        .maxHealth = dto->maxHealth,
        .attack = dto->attack,
        .defense = dto->defense,
        .gold = dto->gold,
        .experience = dto->experience,
        .type = dto->type,
        .musicFilename = dto->musicFilename
    };
    std::shared_ptr<Monster> updateMonster = nullptr;
    try {
        updateMonster = std::make_shared<Monster>(creationInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
    }
    return updateMonster;
}

}  // namespace monstereditor::controllers
