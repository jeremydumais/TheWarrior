#include "player.hpp"
#include "boost/algorithm/string.hpp"
#include <stdexcept>

namespace thewarrior::models {

Player::Player(const std::string &name)
    : m_name(name)
{
    validateName(name);
}

const std::string &Player::getName() const
{
    return m_name;
}

unsigned int Player::getLevel() const
{
    return m_level;
}

std::shared_ptr<Inventory> Player::getInventory()
{
    return m_inventory;
}

PlayerEquipment& Player::getEquipment()
{
    return m_equipment;
}

PlayerStats Player::getStats() const
{
    PlayerStats stats {
        .attack = m_bonusAttackFromLevel,
        .defense = m_bonusDefenseFromLevel,
        .health = m_health,
        .maxHealth = m_bonusHealthFromLevel
    };
    //Add equipment stats
    if (m_equipment.getMainHand().has_value()) {
        stats.attack += m_equipment.getMainHand()->getAttackGain();
    }
    if (m_equipment.getSecondaryHandType() != SecondaryHandType::None) {
        if (m_equipment.getSecondaryHandType() == SecondaryHandType::Weapon) {
            stats.attack += boost::get<WeaponItem>(m_equipment.getSecondaryHand().get()).getAttackGain();
        }
        else {
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

int Player::getGold() const
{
    return m_gold;
}

float Player::getOptionalArmorItemDefense(const boost::optional<ArmorItem> &item)
{
    return item.has_value() ? item->getDefenseGain() : 0.0F;
}

void Player::setName(const std::string &name)
{
    validateName(name);
    m_name = name;
}

void Player::setLevel(unsigned int level)
{
    m_level = level;
}

void Player::incrementLevel()
{
    m_level++;
}

void Player::validateName(const std::string &name) const
{
    if (boost::trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be null or empty.");
    }
}

} // namespace thewarrior::models
