#pragma once

#include <string>
#include <utility>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

struct MonsterCreationInfo {
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
};

class Monster {
 public:
    explicit Monster(MonsterCreationInfo info);
    bool operator==(const Monster &other) const;
    bool operator!=(const Monster &other) const;
    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    std::pair<int, int> getHealthRange() const;
    int getMaxHealth() const;
    int getHealth() const;
    float getHealthRatio() const;
    float getAttack() const;
    float getDefense() const;
    std::pair<int, int> getGoldRewardRange() const;
    std::pair<int, int> getExperienceRewardRange() const;
    bool isDead() const;
    void setId(const std::string &id);
    void setName(const std::string &name);
    void setTextureName(const std::string &textureName);
    void setTextureIndex(int textureIndex);
    void setHealthRange(std::pair<int, int> value);
    void setMaxHealth(int value);
    void setHealth(int value);
    void setAttack(float value);
    void setDefense(float value);
    void setGoldRewardRange(std::pair<int, int> value);
    void setExperienceRewardRange(std::pair<int, int> value);
    void reduceHealth(int amount);
    void restoreHealth(int amount);
    static void validateId(const std::string &id, const std::string &field = "id");

 private:
    friend class boost::serialization::access;
    Monster() = default;  // Needed for deserialization
    std::string m_id = "";
    std::string m_name = "";
    std::string m_textureName = "";
    int m_textureIndex = -1;
    std::pair<int, int> m_healthRange = std::pair<int, int>(0, 0);
    int m_maxHealth = 1;
    int m_health = 1;
    float m_attack = 1.0F;
    float m_defense = 1.0F;
    std::pair<int, int> m_gold = std::pair<int, int>(0, 0);
    std::pair<int, int> m_experience = std::pair<int, int>(0, 0);
    // Serialization method
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m_id;
        ar & m_name;
        ar & m_textureName;
        ar & m_textureIndex;
        ar & m_healthRange;
        ar & m_maxHealth;
        ar & m_health;
        ar & m_attack;
        ar & m_defense;
        ar & m_gold;
        if (version > 1) {
            ar & m_experience;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Monster, 2)
