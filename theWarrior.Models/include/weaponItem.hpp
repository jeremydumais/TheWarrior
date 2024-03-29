#pragma once

#include <string>
#include "item.hpp"
#include "itemType.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

enum class WeaponBodyPart {
    MainHand,
    SecondaryHand
};

struct WeaponItemCreationInfo : public ItemCreationInfo {
    float attackGain;
    WeaponBodyPart slotInBodyPart;
};

class WeaponItem : public Item {
 public:
    WeaponItem();  // Used only for Boost Serialization
    explicit WeaponItem(const WeaponItemCreationInfo &itemInfo);
    ~WeaponItem() override = default;
    WeaponItem(const WeaponItem &) = default;
    WeaponItem(WeaponItem &&) = default;
    WeaponItem &operator=(const WeaponItem &) = default;
    WeaponItem &operator=(WeaponItem &&) = default;
    bool equals(const Item &other) const override;
    ItemType getType() const override;
    float getAttackGain() const;
    WeaponBodyPart getSlotInBodyPart() const;
    void setAttackGain(float value);
    void setSlotInBodyPart(WeaponBodyPart value);
    static std::string getBodyPartAsString(WeaponBodyPart bodyPart);

 protected:
    friend class boost::serialization::access;
    float m_attackGain;
    WeaponBodyPart m_slotInBodyPart;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & boost::serialization::base_object<Item>(*this);
        ar & m_attackGain;
        ar & m_slotInBodyPart;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::WeaponItem, 0)
