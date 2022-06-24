#include "playerEquipment.hpp"

PlayerEquipment::PlayerEquipment()
    : m_mainHand(boost::none),
      m_secondaryHand(boost::none),
      m_head(boost::none),
      m_head(boost::none),
      m_head(boost::none),
      m_head(boost::none),
      m_head(boost::none),
      m_head(boost::none),
{
}

const boost::optional<WeaponItem> &PlayerEquipment::getMainHand() const
{
    return m_mainHand;
}

const boost::optional<boost::variant<WeaponItem, ArmorItem>> &PlayerEquipment::getSecondaryHand() const
{
    return m_secondaryHand;
}

const boost::optional<ArmorItem> &PlayerEquipment::getHead() const
{
    return m_head;
}

const boost::optional<ArmorItem> &PlayerEquipment::getUpperBody() const
{
    return m_upperBody;
}

const boost::optional<ArmorItem> &PlayerEquipment::getLowerBody() const
{
    return m_lowerBody;
}

const boost::optional<ArmorItem> &PlayerEquipment::getHands() const
{
    return m_hands;
}

const boost::optional<ArmorItem> &PlayerEquipment::getFeets() const
{
    return m_feets;
}