#pragma once

#include "inventory.hpp"
#include "playerEquipment.hpp"
#include "playerStats.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <memory>
#include <string>

namespace thewarrior::models {

class Player
{
public:
    explicit Player(const std::string &name);
    virtual ~Player() = default;
    const std::string &getName() const;
    unsigned int getLevel() const;
    std::shared_ptr<Inventory> getInventory();
    PlayerEquipment &getEquipment();
    PlayerStats getStats() const;
    int getGold() const;
    void setName(const std::string &name);
    void setLevel(unsigned int level);
    void incrementLevel();
    //reduceHealth
    //restoreHealth
    void addGold(int value);
    void removeGold(int value);
protected:
    unsigned int m_level = 1;
    int m_health = 10;
    float m_bonusAttackFromLevel = 0.5F;
    float m_bonusDefenseFromLevel = 0.5F;
    int m_bonusHealthFromLevel = 10;
    int m_gold = 0;
private:
    friend class boost::serialization::access;
    Player() = default; //Needed for deserialization
    std::string m_name;
    std::shared_ptr<Inventory> m_inventory = std::make_shared<Inventory>();
    PlayerEquipment m_equipment;
    void validateName(const std::string &name) const;
    static float getOptionalArmorItemDefense(const boost::optional<ArmorItem> &item);
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
        //To define
    }
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Player, 0)
