#include "monster.hpp"
#include <boost/algorithm/string.hpp>
#include <stdexcept>

void validateMonsterName(const std::string &name);
void validateMonsterTextureName(const std::string &textureName);
void validateMonsterTextureIndex(const int textureIndex);

Monster::Monster(MonsterCreationInfo info)
    : m_name(info.name),
      m_textureName(info.textureName),
      m_textureIndex(info.textureIndex),
      m_health(info.health),
      m_attack(info.attack),
      m_defense(info.defense),
      m_goldMinimum(info.goldMinimum),
      m_goldMaximum(info.goldMaximum)
{
    validateMonsterName(m_name);
    validateMonsterTextureName(m_textureName);
    validateMonsterTextureIndex(m_textureIndex);
}

const std::string &Monster::getName() const
{
    return m_name;
}

const std::string& Monster::getTextureName() const
{
    return m_textureName;
}

int Monster::getTextureIndex() const
{
    return m_textureIndex;
}

int Monster::getHealth() const
{
    return m_health;
}

float Monster::getAttack() const
{
    return m_attack;
}

float Monster::getDefense() const
{
    return m_defense;
}

std::pair<int, int> Monster::getGoldRewardRange() const
{
    return { m_goldMinimum, m_goldMaximum };
}

void Monster::setName(const std::string &name)
{
    validateMonsterName(name);
    m_name = name;
}

void Monster::setTextureName(const std::string &textureName)
{
    validateMonsterTextureName(textureName);
    m_textureName = textureName;
}

void Monster::setTextureIndex(int textureIndex)
{
    validateMonsterTextureIndex(textureIndex);
    m_textureIndex = textureIndex;
}

void Monster::setHealth(int value)
{
    m_health = value;
}

void Monster::setAttack(float value)
{
    m_attack = value;
}

void Monster::setDefense(float value)
{
    m_defense = value;
}

void Monster::setGoldRewardRange(int minimum, int maximum)
{
    if (minimum < 0) {
        throw std::invalid_argument("gold reward minimum cannot be a negative number.");
    }
    if (maximum < minimum) {
        throw std::invalid_argument("gold reward maximum must be greater or equal to the minimum.");
    }
    m_goldMinimum = minimum;
    m_goldMaximum = maximum;
}

void validateMonsterName(const std::string &name)
{
    if (boost::trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}

void validateMonsterTextureName(const std::string &textureName)
{
    if (boost::trim_copy(textureName).empty()) {
        throw std::invalid_argument("texture name cannot be null or empty.");
    }
}

void validateMonsterTextureIndex(const int textureIndex)
{
    if (textureIndex < 0) {
        throw std::invalid_argument("texture index cannot be a negative number.");
    }
}
