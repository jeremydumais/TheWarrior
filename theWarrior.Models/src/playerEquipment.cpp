#include "playerEquipment.hpp"
#include <fmt/format.h>
#include <stdexcept>

SecondaryHandType getVariantEquipmentType(VariantEquipment variantEquipment);
void validateMainHand(boost::optional<WeaponItem> mainHand);
void validateSecondaryHand(VariantEquipment secondaryHand);
void validateHead(boost::optional<ArmorItem> head);
void validateUpperBody(boost::optional<ArmorItem> upperBody);
void validateLowerBody(boost::optional<ArmorItem> lowerBody);
void validateHands(boost::optional<ArmorItem> hands);
void validateFeet(boost::optional<ArmorItem> feet);

PlayerEquipment::PlayerEquipment()
    : m_mainHand(boost::none),
      m_secondaryHand(boost::none),
      m_head(boost::none),
      m_upperBody(boost::none),
      m_lowerBody(boost::none),
      m_hands(boost::none),
      m_feet(boost::none)
{
}

PlayerEquipment::PlayerEquipment(boost::optional<WeaponItem> mainHand,
                    VariantEquipment secondaryHand,
                    boost::optional<ArmorItem> head,
                    boost::optional<ArmorItem> upperBody,
                    boost::optional<ArmorItem> lowerBody,
                    boost::optional<ArmorItem> hands,
                    boost::optional<ArmorItem> feet)
    : m_mainHand(mainHand),
      m_secondaryHand(secondaryHand),
      m_head(head),
      m_upperBody(upperBody),
      m_lowerBody(lowerBody),
      m_hands(hands),
      m_feet(feet)
{
    validateMainHand(mainHand);
    validateSecondaryHand(secondaryHand);
    validateHead(head);
    validateUpperBody(upperBody);
    validateLowerBody(lowerBody);
    validateHands(hands);
    validateFeet(feet); 
}

const boost::optional<WeaponItem> &PlayerEquipment::getMainHand() const
{
    return m_mainHand;
}

const boost::optional<boost::variant<WeaponItem, ArmorItem>> &PlayerEquipment::getSecondaryHand() const
{
    return m_secondaryHand;
}

SecondaryHandType PlayerEquipment::getSecondaryHandType() const
{
    return getVariantEquipmentType(m_secondaryHand);
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

const boost::optional<ArmorItem> &PlayerEquipment::getFeet() const
{
    return m_feet;
}

void PlayerEquipment::setMainHand(boost::optional<WeaponItem> weapon)
{
    validateMainHand(weapon);
    m_mainHand = weapon;
}

void PlayerEquipment::setSecondaryHand(VariantEquipment weaponOrArmor)
{
    validateSecondaryHand(weaponOrArmor);
    m_secondaryHand = weaponOrArmor;
}

void PlayerEquipment::setHead(boost::optional<ArmorItem> armor)
{
    validateHead(armor);
    m_head = armor;
}

void PlayerEquipment::setUpperBody(boost::optional<ArmorItem> armor)
{
    validateUpperBody(armor);
    m_upperBody = armor;
}

void PlayerEquipment::setLowerBody(boost::optional<ArmorItem> armor)
{
    validateLowerBody(armor);
    m_lowerBody = armor;
}

void PlayerEquipment::setHands(boost::optional<ArmorItem> armor)
{
    validateHands(armor);
    m_hands = armor;
}

void PlayerEquipment::setFeet(boost::optional<ArmorItem> armor)
{
    validateFeet(armor);
    m_feet = armor;
}

SecondaryHandType getVariantEquipmentType(VariantEquipment variantEquipment)
{
    if (!variantEquipment.has_value()) {
        return SecondaryHandType::None;
    }
    return variantEquipment->which() == 0 ? SecondaryHandType::Weapon : SecondaryHandType::Armor;
}

void validateMainHand(boost::optional<WeaponItem> mainHand)
{
    if (mainHand.has_value() && mainHand->getSlotInBodyPart() != WeaponBodyPart::MainHand) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} in main hand.", mainHand->getName()));
    }
}

void validateSecondaryHand(VariantEquipment secondaryHand)
{
    auto secondaryHandType = getVariantEquipmentType(secondaryHand);
    if (secondaryHandType == SecondaryHandType::Weapon) {
        auto weapon = boost::get<WeaponItem>(secondaryHand.get());
        if (weapon.getSlotInBodyPart() != WeaponBodyPart::SecondaryHand) {
            throw std::invalid_argument(fmt::format("Cannot slot {0} in secondary hand.", weapon.getName()));
        }
    }
    if (secondaryHandType == SecondaryHandType::Armor) {
        auto armor = boost::get<ArmorItem>(secondaryHand.get());
        if (armor.getSlotInBodyPart() != ArmorBodyPart::SecondaryHand) {
            throw std::invalid_argument(fmt::format("Cannot slot {0} in secondary hand.", armor.getName()));
        }
    }
}

void validateHead(boost::optional<ArmorItem> head)
{
    if (head.has_value() && head->getSlotInBodyPart() != ArmorBodyPart::Head) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} on head.", head->getName()));
    }
}

void validateUpperBody(boost::optional<ArmorItem> upperBody)
{
    if (upperBody.has_value() && upperBody->getSlotInBodyPart() != ArmorBodyPart::UpperBody) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} on upper body.", upperBody->getName()));
    }
}

void validateLowerBody(boost::optional<ArmorItem> lowerBody)
{
    if (lowerBody.has_value() && lowerBody->getSlotInBodyPart() != ArmorBodyPart::LowerBody) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} on lower body.", lowerBody->getName()));
    }
}

void validateHands(boost::optional<ArmorItem> hands)
{
    if (hands.has_value() && hands->getSlotInBodyPart() != ArmorBodyPart::Hands) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} in hands.", hands->getName()));
    }
}

void validateFeet(boost::optional<ArmorItem> feet)
{
    if (feet.has_value() && feet->getSlotInBodyPart() != ArmorBodyPart::Feet) {
        throw std::invalid_argument(fmt::format("Cannot slot {0} in feet.", feet->getName()));
    }
}