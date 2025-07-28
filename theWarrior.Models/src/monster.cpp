#include <fmt/core.h>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <utility>
#include "monster.hpp"
#include <boost/algorithm/string.hpp>

namespace thewarrior::models {

void validateMonsterName(const std::string &name);
void validateMonsterTextureName(const std::string &textureName);
void validateMonsterTextureIndex(const int textureIndex);
void validateMonsterGoldReward(const int minimum, const int maximum);


Monster::Monster(MonsterCreationInfo info)
    : m_id(info.id),
      m_name(info.name),
      m_textureName(info.textureName),
      m_textureIndex(info.textureIndex),
      m_maxHealth(info.maxHealth),
      m_health(info.maxHealth),
      m_attack(info.attack),
      m_defense(info.defense),
      m_goldMinimum(info.goldMinimum),
      m_goldMaximum(info.goldMaximum) {
    Monster::validateId(m_id);
    validateMonsterName(m_name);
    validateMonsterTextureName(m_textureName);
    validateMonsterTextureIndex(m_textureIndex);
    validateMonsterGoldReward(m_goldMinimum, m_goldMaximum);
}

bool Monster::operator==(const Monster &other) const {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;
    }
    return this->m_id == other.m_id &&
           this->m_name == other.m_name &&
           this->m_textureName == other.m_textureName &&
           this->m_textureIndex == other.m_textureIndex &&
           this->m_maxHealth == other.m_maxHealth &&
           this->m_health == other.m_health &&
           this->m_attack == other.m_attack &&
           this->m_defense == other.m_defense &&
           this->m_goldMinimum == other.m_goldMinimum &&
           this->m_goldMaximum == other.m_goldMaximum;
}

bool Monster::operator!=(const Monster &other) const {
    return !(*this == other);
}

const std::string& Monster::getId() const {
    return m_id;
}

const std::string &Monster::getName() const {
    return m_name;
}

const std::string& Monster::getTextureName() const {
    return m_textureName;
}

int Monster::getTextureIndex() const {
    return m_textureIndex;
}

int Monster::getMaxHealth() const {
    return m_maxHealth;
}

int Monster::getHealth() const {
    return m_health;
}

float Monster::getHealthRatio() const {
    if (m_maxHealth != 0) {
        float result = static_cast<float>(m_health) / static_cast<float>(m_maxHealth);
        return result;
    } else {
        return 0.0F;
    }
}

float Monster::getAttack() const {
    return m_attack;
}

float Monster::getDefense() const {
    return m_defense;
}

std::pair<int, int> Monster::getGoldRewardRange() const {
    return { m_goldMinimum, m_goldMaximum };
}

bool Monster::isDead() const {
    return m_health == 0;
}

void Monster::setId(const std::string &id) {
    validateId(id);
    m_id = id;
}

void Monster::setName(const std::string &name) {
    validateMonsterName(name);
    m_name = name;
}

void Monster::setTextureName(const std::string &textureName) {
    validateMonsterTextureName(textureName);
    m_textureName = textureName;
}

void Monster::setTextureIndex(int textureIndex) {
    validateMonsterTextureIndex(textureIndex);
    m_textureIndex = textureIndex;
}

void Monster::setMaxHealth(int value) {
    if (m_health > value) {
        m_health = value;
    }
    m_maxHealth = value;
}

void Monster::setHealth(int value) {
    if (value > m_maxHealth) {
        m_health = m_maxHealth;
    } else {
        m_health = value;
    }
}

void Monster::setAttack(float value) {
    m_attack = value;
}

void Monster::setDefense(float value) {
    m_defense = value;
}

void Monster::setGoldRewardRange(int minimum, int maximum) {
    validateMonsterGoldReward(minimum, maximum);
    m_goldMinimum = minimum;
    m_goldMaximum = maximum;
}

void Monster::reduceHealth(int amount) {
    if (amount > m_health) {
        m_health = 0;
    } else {
        m_health -= amount;
    }
}

void Monster::restoreHealth(int amount) {
    if (m_health + amount > m_maxHealth) {
        m_health = m_maxHealth;
    } else {
        m_health += amount;
    }
}

void Monster::validateId(const std::string &id, const std::string &field) {
    std::string sanitizedId { boost::trim_copy(id) };
    if (sanitizedId.empty()) {
        throw std::invalid_argument(fmt::format("{0} cannot be empty.", field));
    }
    if (sanitizedId.length() != 6) {
        throw std::invalid_argument(fmt::format("{0} must be 6 characters long.", field));
    }
}

void validateMonsterName(const std::string &name) {
    if (boost::trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}

void validateMonsterTextureName(const std::string &textureName) {
    if (boost::trim_copy(textureName).empty()) {
        throw std::invalid_argument("texture name cannot be null or empty.");
    }
}

void validateMonsterTextureIndex(const int textureIndex) {
    if (textureIndex < 0) {
        throw std::invalid_argument("texture index cannot be a negative number.");
    }
}

void validateMonsterGoldReward(const int minimum, const int maximum) {
    if (minimum < 0) {
        throw std::invalid_argument("gold reward minimum cannot be a negative number.");
    }
    if (maximum < minimum) {
        throw std::invalid_argument("gold reward maximum must be greater or equal to the minimum.");
    }
}

}  // namespace thewarrior::models
