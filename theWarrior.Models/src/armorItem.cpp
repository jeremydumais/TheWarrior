#include "armorItem.hpp"

//Used only for Boost Serialization
ArmorItem::ArmorItem()
    : Item(),
      m_defenseGain(0.0F),
      m_slotInBodyPart(ArmorBodyPart::Head)
{
}

ArmorItem::ArmorItem(const ArmorItemCreationInfo &itemInfo)
    : Item(itemInfo),
      m_defenseGain(itemInfo.defenseGain),
      m_slotInBodyPart(itemInfo.slotInBodyPart)
{
}

bool ArmorItem::equals(const Item &other) const 
{
    if (typeid(*this).hash_code() != typeid(other).hash_code()) {
        return false;    
    }
    const ArmorItem *otherArmorItem = dynamic_cast<const ArmorItem *>(&other);
    if (otherArmorItem == nullptr) {
        return false;
    }
    return this->m_defenseGain == otherArmorItem->m_defenseGain &&
           this->m_slotInBodyPart == otherArmorItem->m_slotInBodyPart &&
           Item::equals(other);
}

std::string ArmorItem::getType() const
{
    return "Armor";
}

float ArmorItem::getDefenseGain() const
{
    return m_defenseGain;
}

ArmorBodyPart ArmorItem::getSlotInBodyPart() const
{
    return m_slotInBodyPart;
}

void ArmorItem::setDefenseGain(float value)
{
    m_defenseGain = value;
}

void ArmorItem::setSlotInBodyPart(ArmorBodyPart value)
{
    m_slotInBodyPart = value;
}
