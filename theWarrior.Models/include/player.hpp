#pragma once

#include "inventory.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <string>

class Player
{
public:
    explicit Player(const std::string &name);
    const std::string &getName() const;
    unsigned int getLevel() const;
    void setName(const std::string &name);
    void setLevel(unsigned int level);
    void incrementLevel();
private:
    friend class boost::serialization::access;
    Player() = default; //Needed for deserialization
    std::string m_name;
    unsigned int m_level;
    Inventory m_inventory;
    void validateName(const std::string &name) const;
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
        //To define
    }
};
BOOST_CLASS_VERSION(Player, 0)