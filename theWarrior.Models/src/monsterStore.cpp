#include "monsterStore.hpp"
#include <algorithm>

using namespace std;

namespace thewarrior::models {

MonsterStore::MonsterStore()
    : m_lastError(""),
      m_textureContainer(TextureContainer()),
      m_monsters(unordered_map<string, std::shared_ptr<Monster>>())
{
}

const std::string &MonsterStore::getLastError() const
{
    return m_lastError;
}

size_t MonsterStore::getMonsterCount() const
{
    return m_monsters.size();
}

std::vector<std::shared_ptr<Monster>> MonsterStore::getMonsters() const
{
    std::vector<std::shared_ptr<Monster>> retval {};
    std::transform(m_monsters.begin(),
                   m_monsters.end(),
                   std::back_inserter(retval),
                   [] (std::pair<const std::string &, std::shared_ptr<Monster>>monster) {
                       return std::reference_wrapper(monster.second);
                   });
    std::sort(retval.begin(), retval.end(), [](std::shared_ptr<Monster> a,
                                               std::shared_ptr<Monster> b) {
        return a->getId() < b->getId();
    });
    return retval;
}

const std::shared_ptr<const Monster> MonsterStore::findMonster(const std::string &id) const
{
    const auto iter = m_monsters.find(id);
    if (iter != m_monsters.end()) {
        return iter->second;
    }
    else {
        return nullptr;
    }
}

bool MonsterStore::isMonsterExists(const std::string &id) const
{
    return this->findMonster(id) != nullptr;
}


bool MonsterStore::addMonster(std::shared_ptr<Monster> monster)
{
    bool wasInserted = m_monsters.insert({ monster->getId(), monster }).second;
    return wasInserted;
}

bool MonsterStore::replaceMonster(const string oldId, std::shared_ptr<Monster> monster)
{
    //Check if the old monster name specified exist
    const auto iter = m_monsters.find(oldId);
    if (iter == m_monsters.end()) {
        return false;
    }
    if (m_monsters.erase(oldId) == 0) {
        return false;
    }
    bool wasInserted = m_monsters.insert({ monster->getId(), monster }).second;
    return wasInserted;
}

bool MonsterStore::removeMonster(const std::string &id)
{
    if (m_monsters.erase(id) == 0) {
        return false;
    }
    return true;
}

const TextureContainer &MonsterStore::getTextureContainer() const
{
    return m_textureContainer;
}

TextureContainer &MonsterStore::getTextureContainerForEdition()
{
    return m_textureContainer;
}

} // namespace thewarrior::models
