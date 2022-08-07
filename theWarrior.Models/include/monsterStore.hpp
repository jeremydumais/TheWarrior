#pragma once

#include "monster.hpp"
#include "textureContainer.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/version.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace thewarrior::models {

class MonsterStore
{
public:
    MonsterStore();
    const std::string &getLastError() const;
    size_t getMonsterCount() const;
    std::vector<std::shared_ptr<Monster>> getMonsters() const;
    const std::shared_ptr<const Monster> findMonster(const std::string &id) const;
    bool isMonsterExists(const std::string &id) const;
    bool addMonster(std::shared_ptr<Monster> monster);
    bool replaceMonster(const std::string oldId, std::shared_ptr<Monster> monster);
    bool removeMonster(const std::string &id);
    const TextureContainer &getTextureContainer() const;
    TextureContainer &getTextureContainerForEdition();
private:
    friend class boost::serialization::access;
    std::string m_lastError;
    TextureContainer m_textureContainer;
    std::unordered_map<std::string, std::shared_ptr<Monster>> m_monsters;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_textureContainer;
        ar & m_monsters;
    }
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MonsterStore, 0)
