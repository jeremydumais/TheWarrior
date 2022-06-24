#pragma once

#include "armorItem.hpp"
#include "weaponItem.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/version.hpp>

typedef boost::optional<boost::variant<WeaponItem, ArmorItem>> VariantEquipment;

class PlayerEquipment
{
public:
    PlayerEquipment(); //Used only for Boost Serialization
    const boost::optional<WeaponItem> &getMainHand() const;
    const VariantEquipment &getSecondaryHand() const;
    const boost::optional<ArmorItem> &getHead() const;
    const boost::optional<ArmorItem> &getUpperBody() const;
    const boost::optional<ArmorItem> &getLowerBody() const;
    const boost::optional<ArmorItem> &getHands() const;
    const boost::optional<ArmorItem> &getFeets() const;
private:
    friend class boost::serialization::access;
    boost::optional<WeaponItem> m_mainHand;
    VariantEquipment m_secondaryHand;
    boost::optional<ArmorItem> m_head;
    boost::optional<ArmorItem> m_upperBody;
    boost::optional<ArmorItem> m_lowerBody;
    boost::optional<ArmorItem> m_hands;
    boost::optional<ArmorItem> m_feets;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        //To code later
    }
};