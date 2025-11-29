#pragma once

#include <memory>
#include <string>
#include <boost/serialization/array_wrapper.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "inventory.hpp"
#include "playerEquipment.hpp"
#include "playerStats.hpp"

namespace thewarrior::models {

enum class PlayerFacing { Left, Up, Right, Down };

class Player {
 public:
    explicit Player(const std::string &name);
    virtual ~Player() = default;
    const std::string &getName() const;
    unsigned int getLevel() const;
    std::shared_ptr<Inventory> getInventory();
    PlayerEquipment &getEquipment();
    PlayerStats getStats() const;
    bool isDead() const;
    int getGold() const;
    int getExperience() const;
    bool isFacing(PlayerFacing direction);
    bool isClimbing() const;
    void setName(const std::string &name);
    void setLevel(unsigned int level);
    void setFacing(PlayerFacing direction);
    void setClimbing(bool isPlayerClimbing);
    void incrementLevel();
    void reduceHealth(int amount);
    void restoreHealth(int amount);
    void addGold(int value);
    void removeGold(int value);
    void addExperience(int value);

 protected:
    unsigned int m_level = 1;
    unsigned int m_maxLevel = 10;
    int m_health = 15;
    int m_maxHealth = 15;
    int m_strength = 4;
    int m_agility = 4;
    int m_bonusHealthFromLevel = 10;
    int m_gold = 0;
    int m_experience = 0;

 private:
    friend class boost::serialization::access;
    Player() = default;  // Needed for deserialization
    std::string m_name;
    std::shared_ptr<Inventory> m_inventory = std::make_shared<Inventory>();
    PlayerEquipment m_equipment;
    PlayerFacing m_playerFacing = PlayerFacing::Up;
    bool m_isInClimbingMode = false;
    void validateName(const std::string &name) const;
    int getMaxHealthByLevel(unsigned int level);
    int getStrengthByLevel(unsigned int level);
    int getAgilityByLevel(unsigned int level);
    int getExperienceByLevel(unsigned int level);
    static float getOptionalArmorItemDefense(const boost::optional<ArmorItem> &item);
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_name;
        ar & m_level;
        ar & m_maxLevel;
        ar & m_health;
        ar & m_maxHealth;
        ar & m_strength;
        ar & m_agility;
        ar & m_bonusHealthFromLevel;
        ar & m_gold;
        ar & m_experience;
        ar & *m_inventory;
        ar & m_equipment;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Player, 0)
