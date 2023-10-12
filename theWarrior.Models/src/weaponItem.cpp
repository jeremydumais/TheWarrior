#include "weaponItem.hpp"

namespace thewarrior::models {

// Used only for Boost Serialization
WeaponItem::WeaponItem()
    : Item(),
      m_attackGain(0.0F),
      m_slotInBodyPart(WeaponBodyPart::MainHand) {
}

WeaponItem::WeaponItem(const WeaponItemCreationInfo &itemInfo)
    : Item(itemInfo),
      m_attackGain(itemInfo.attackGain),
      m_slotInBodyPart(itemInfo.slotInBodyPart) {
}

bool WeaponItem::equals(const Item &other) const {
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;
    }
    const WeaponItem *otherWeaponItem = dynamic_cast<const WeaponItem *>(&other);
    if (otherWeaponItem == nullptr) {
        return false;
    }
    return this->m_attackGain == otherWeaponItem->m_attackGain &&
           this->m_slotInBodyPart == otherWeaponItem->m_slotInBodyPart &&
           Item::equals(other);
}

ItemType WeaponItem::getType() const {
    return ItemType::Weapon;
}

float WeaponItem::getAttackGain() const {
    return m_attackGain;
}

WeaponBodyPart WeaponItem::getSlotInBodyPart() const {
    return m_slotInBodyPart;
}

void WeaponItem::setAttackGain(float value) {
    m_attackGain = value;
}

void WeaponItem::setSlotInBodyPart(WeaponBodyPart value) {
    m_slotInBodyPart = value;
}

std::string WeaponItem::getBodyPartAsString(WeaponBodyPart bodyPart) {
    switch (bodyPart) {
    case WeaponBodyPart::MainHand:
        return "Main hand";
    case WeaponBodyPart::SecondaryHand:
        return "Secondary hand";
    }
    return "";
}

}  // namespace thewarrior::models
