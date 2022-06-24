#pragma once

#include "armorItem.hpp"
#include "weaponItem.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/version.hpp>

typedef boost::optional<boost::variant<WeaponItem, ArmorItem>> VariantEquipment;

enum class SecondaryHandType {
    None,
    Weapon,
    Armor
};

class PlayerEquipment
{
public:
    PlayerEquipment();
    PlayerEquipment(boost::optional<WeaponItem> mainHand,
                    VariantEquipment secondaryHand,
                    boost::optional<ArmorItem> head,
                    boost::optional<ArmorItem> upperBody,
                    boost::optional<ArmorItem> lowerBody,
                    boost::optional<ArmorItem> hands,
                    boost::optional<ArmorItem> feet);
    const boost::optional<WeaponItem> &getMainHand() const;
    const VariantEquipment &getSecondaryHand() const;
    const boost::optional<ArmorItem> &getHead() const;
    const boost::optional<ArmorItem> &getUpperBody() const;
    const boost::optional<ArmorItem> &getLowerBody() const;
    const boost::optional<ArmorItem> &getHands() const;
    const boost::optional<ArmorItem> &getFeet() const;
    void setMainHand(boost::optional<WeaponItem> weapon);
    void setSecondaryHand(VariantEquipment weaponOrArmor);
private:
    friend class boost::serialization::access;
    boost::optional<WeaponItem> m_mainHand;
    VariantEquipment m_secondaryHand;
    boost::optional<ArmorItem> m_head;
    boost::optional<ArmorItem> m_upperBody;
    boost::optional<ArmorItem> m_lowerBody;
    boost::optional<ArmorItem> m_hands;
    boost::optional<ArmorItem> m_feet;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        //To code later
    }
};