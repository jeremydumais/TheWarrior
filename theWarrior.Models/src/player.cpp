#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include "player.hpp"
#include "boost/algorithm/string.hpp"

namespace thewarrior::models {

Player::Player(const std::string &name)
    : m_name(name) {
    validateName(name);
}

const std::string &Player::getName() const {
    return m_name;
}

unsigned int Player::getLevel() const {
    return m_level;
}

std::shared_ptr<Inventory> Player::getInventory() {
    return m_inventory;
}

PlayerEquipment& Player::getEquipment() {
    return m_equipment;
}

PlayerStats Player::getStats() const {
    PlayerStats stats {
        .attack = static_cast<float>(m_strength),
        .defense = static_cast<float>(m_agility),
        .health = m_health,
        .maxHealth = m_maxHealth,
        .experience = m_experience
    };
    // Add equipment stats
    if (m_equipment.getMainHand().has_value()) {
        stats.attack += m_equipment.getMainHand()->getAttackGain();
    }
    if (m_equipment.getSecondaryHandType() != SecondaryHandType::None) {
        if (m_equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            stats.attack += boost::get<WeaponItem>(m_equipment.getSecondaryHand().get()).getAttackGain();
        } else {
            stats.defense += boost::get<ArmorItem>(m_equipment.getSecondaryHand().get()).getDefenseGain();
        }
    }
    stats.defense += Player::getOptionalArmorItemDefense(m_equipment.getHead());
    stats.defense += Player::getOptionalArmorItemDefense(m_equipment.getUpperBody());
    stats.defense += Player::getOptionalArmorItemDefense(m_equipment.getLowerBody());
    stats.defense += Player::getOptionalArmorItemDefense(m_equipment.getHands());
    stats.defense += Player::getOptionalArmorItemDefense(m_equipment.getFeet());
    return stats;
}

int Player::getGold() const {
    return m_gold;
}

int Player::getExperience() const {
    return m_experience;
}

float Player::getOptionalArmorItemDefense(const boost::optional<ArmorItem> &item) {
    return item.has_value() ? item->getDefenseGain() : 0.0F;
}

void Player::setName(const std::string &name) {
    validateName(name);
    m_name = name;
}

void Player::setLevel(unsigned int level) {
    m_level = level;
}

void Player::incrementLevel() {
    if (m_level < m_maxLevel) {
        m_level++;
        // Set new HP, Attack and Defense
        m_maxHealth = getMaxHealthByLevel(m_level);
        m_health = m_maxHealth;
        m_strength = getStrengthByLevel(m_level);
        m_agility = getAgilityByLevel(m_level);
    }
}

void Player::reduceHealth(int amount) {
    if (amount > m_health) {
        m_health = 0;
    } else {
        m_health -= amount;
    }
}

void Player::restoreHealth(int amount) {
    if (m_health + amount > m_maxHealth) {
        m_health = m_maxHealth;
    } else {
        m_health += amount;
    }
}

void Player::addGold(int value) {
    int intMax = std::numeric_limits<int>::max();
    if (intMax - m_gold >= value) {
        m_gold += value;
    } else {
        m_gold = intMax;
    }
}

void Player::removeGold(int value) {
    if (m_gold - value >= 0) {
        m_gold -= value;
    } else {
        m_gold = 0;
    }
}

void Player::addExperience(int value) {
    int intMax = std::numeric_limits<int>::max();
    if (intMax - m_experience >= value) {
        m_experience += value;
    } else {
        m_experience = intMax;
    }
    auto experienceLimitToNextLevel = getExperienceByLevel(m_level + 1);
    if (m_experience >= experienceLimitToNextLevel) {
        incrementLevel();
    }
}

void Player::validateName(const std::string &name) const {
    if (boost::trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}

int Player::getMaxHealthByLevel(unsigned int level) {
    switch (level) {
        case 1:
            return 15;
        case 2:
            return 22;
        case 3:
            return 30;
        case 4:
            return 38;
        case 5:
            return 46;
        case 6:
            return 54;
        case 7:
            return 62;
        case 8:
            return 70;
        case 9:
            return 78;
        case 10:
            return 86;
        default:
            return 86;
    }
}

int Player::getStrengthByLevel(unsigned int level) {
    switch (level) {
        case 1:
            return 4;
        case 2:
            return 5;
        case 3:
            return 7;
        case 4:
            return 10;
        case 5:
            return 13;
        case 6:
            return 16;
        case 7:
            return 19;
        case 8:
            return 22;
        case 9:
            return 25;
        case 10:
            return 28;
        default:
            return 28;
    }
}

int Player::getAgilityByLevel(unsigned int level) {
    switch (level) {
        case 1:
            return 4;
        case 2:
            return 5;
        case 3:
            return 7;
        case 4:
            return 10;
        case 5:
            return 13;
        case 6:
            return 16;
        case 7:
            return 19;
        case 8:
            return 22;
        case 9:
            return 25;
        case 10:
            return 28;
        default:
            return 28;
    }
}

int Player::getExperienceByLevel(unsigned int level) {
    // What experience it takes to reach that level
    switch (level) {
        case 1:
            return 0;
        case 2:
            return 7;
        case 3:
            return 23;
        case 4:
            return 47;
        case 5:
            return 110;
        case 6:
            return 220;
        case 7:
            return 450;
        case 8:
            return 800;
        case 9:
            return 1300;
        case 10:
            return 2200;
        default:
            return 2200;
    }
}

}  // namespace thewarrior::models
