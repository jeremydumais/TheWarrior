#pragma once

#include "inventory.hpp"
#include "playerEquipment.hpp"
#include "playerStats.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <memory>
#include <string>

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
    void setName(const std::string &name);
    void setLevel(unsigned int level);
    void incrementLevel();
protected:
    unsigned int m_level;
    int m_health;
    float m_bonusAttackFromLevel;
    float m_bonusDefenseFromLevel;
    int m_bonusHealthFromLevel;
private:
    friend class boost::serialization::access;
    Player() = default; //Needed for deserialization
    std::string m_name;
    std::shared_ptr<Inventory> m_inventory;
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
BOOST_CLASS_VERSION(Player, 0)