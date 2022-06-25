#pragma once

#include "inventory.hpp"
#include "playerEquipment.hpp"
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
    void setName(const std::string &name);
    void setLevel(unsigned int level);
    void incrementLevel();
private:
    friend class boost::serialization::access;
    Player() = default; //Needed for deserialization
    std::string m_name;
    unsigned int m_level;
    std::shared_ptr<Inventory> m_inventory;
    PlayerEquipment m_equipment;
    void validateName(const std::string &name) const;
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
        //To define
    }
};
BOOST_CLASS_VERSION(Player, 0)