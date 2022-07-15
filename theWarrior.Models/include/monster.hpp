#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <string>

struct MonsterCreationInfo
{
    std::string name;
    std::string textureName;
    int textureIndex;
    int health;
    float attack;
    float defense;
    int goldMinimum;
    int goldMaximum;
};

class Monster
{
public:
    explicit Monster(MonsterCreationInfo info);
    const std::string &getName() const;
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    int getHealth() const;
    float getAttack() const;
    float getDefense() const;
    int getGoldMinimum() const;
    int getGoldMaximum() const;
    void setName(const std::string &name);
private:
    friend class boost::serialization::access;
    Monster() = default; //Needed for deserialization
    std::string m_name = "";
    std::string m_textureName = "";
    int m_textureIndex = -1;
    int m_health = 1;
    float m_attack = 1.0F;
    float m_defense = 1.0F;
    int m_goldMinimum = 0;
    int m_goldMaximum = 0;
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
        //To define
    }
};
BOOST_CLASS_VERSION(Monster, 0)