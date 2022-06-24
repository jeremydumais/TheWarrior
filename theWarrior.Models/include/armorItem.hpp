#pragma once

#include "item.hpp"
#include "itemType.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

enum class ArmorBodyPart
{
    Head,
    UpperBody,
    LowerBody,
    Hands,
    Feet,
    SecondaryHand
};

struct ArmorItemCreationInfo : public ItemCreationInfo
{
    float defenseGain;
    ArmorBodyPart slotInBodyPart;
};

class ArmorItem : public Item
{
public:
    ArmorItem(); //Used only for Boost Serialization
    explicit ArmorItem(const ArmorItemCreationInfo &itemInfo);
    virtual ~ArmorItem() = default;
    ArmorItem(const ArmorItem &) = default;
    ArmorItem(ArmorItem &&) = default;
    ArmorItem &operator=(const ArmorItem &) = default;
    ArmorItem &operator=(ArmorItem &&) = default;
    virtual bool equals(const Item &other) const override;
    virtual ItemType getType() const override;
    float getDefenseGain() const;
    ArmorBodyPart getSlotInBodyPart() const;
    void setDefenseGain(float value);
    void setSlotInBodyPart(ArmorBodyPart value);
protected:
    friend class boost::serialization::access;
    float m_defenseGain;
    ArmorBodyPart m_slotInBodyPart;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & boost::serialization::base_object<Item>(*this);    
        ar & m_defenseGain;
        ar & m_slotInBodyPart;
    }
};
BOOST_CLASS_VERSION(ArmorItem, 0)