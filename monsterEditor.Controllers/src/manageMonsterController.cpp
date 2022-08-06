#include "manageMonsterController.hpp"
#include <fmt/format.h>
#include <memory>
#include <stdexcept>

namespace MonsterEditorControllers
{
ManageMonsterController::ManageMonsterController(std::shared_ptr<MonsterStore> monsterStore)
    : m_monsterStore(monsterStore),
      m_lastError("")
{
}

const std::string& ManageMonsterController::getLastError() const
{
    return m_lastError;
}

std::shared_ptr<MonsterStore> ManageMonsterController::getMonsterStore()
{
    return m_monsterStore;
}

const TextureContainer &ManageMonsterController::getTextureContainer() const
{
    return m_monsterStore->getTextureContainer();
}

std::unique_ptr<MonsterDTO> ManageMonsterController::getMonster(const std::string &id) const
{
    auto monster = m_monsterStore->findMonster(id);
    if (monster != nullptr) {
        auto retval = std::make_unique<MonsterDTO>();
        retval->id = monster->getId();
        retval->name = monster->getName();
        retval->textureName = monster->getTextureName();
        retval->textureIndex = monster->getTextureIndex();
        retval->health = monster->getHealth();
        retval->attack = monster->getAttack();
        retval->defense = monster->getDefense();
        auto [min, max] = monster->getGoldRewardRange();
        retval->goldMinimum = min;
        retval->goldMaximum = max;
        return retval;
    }
    return nullptr;
}

bool ManageMonsterController::addMonster(std::unique_ptr<MonsterDTO> monsterInfo)
{
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
                                      const std::string &oldMonsterId)
{
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

bool ManageMonsterController::deleteMonster(const std::string &monsterId)
{
    if (!m_monsterStore->removeMonster(monsterId)) {
        m_lastError = m_monsterStore->getLastError();
        return false;
    }
    return true;
}

std::shared_ptr<Monster> ManageMonsterController::monsterDTOToMonster(std::unique_ptr<MonsterDTO> dto)
{
    MonsterCreationInfo creationInfo = {
        dto->id,
        dto->name,
        dto->textureName,
        dto->textureIndex,
        dto->health,
        dto->attack,
        dto->defense,
        dto->goldMinimum,
        dto->goldMaximum
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
} // namespace ManageMonsterController
