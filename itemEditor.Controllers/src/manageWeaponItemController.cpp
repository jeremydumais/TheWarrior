#include "manageWeaponItemController.hpp"
#include "boost/algorithm/string.hpp"

ManageWeaponItemController::ManageWeaponItemController(std::shared_ptr<ItemStore> itemStore)
    : ManageItemController(itemStore)
{
}

bool ManageWeaponItemController::validateAttackGain(const std::string &attackGainStr)
{
	if (boost::trim_copy(attackGainStr).empty()) {
        m_lastError = "The attack gain value cannot be empty.";
		return false;
	}
	//Perform the str to float conversion
	try {
		std::stof(attackGainStr);
	}
	catch(const std::invalid_argument &err) {
		m_lastError = "Unable to perform the float conversion of the attack gain.";
		return false;
	}
	catch(const std::out_of_range &err) {
		m_lastError = "The attack gain value is out of range.";
		return false;
	}
	return true;
}


std::unique_ptr<ItemDTO> ManageWeaponItemController::getItem(const std::string &id) const 
{
	auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
		auto weaponItem = dynamic_cast<const WeaponItem *>(item.get());
		if (weaponItem != nullptr) {
			auto retval = std::make_unique<WeaponItemDTO>();
			retval->id = weaponItem->getId();
			retval->name = weaponItem->getName();
			retval->textureName = weaponItem->getTextureName();
			retval->textureIndex = weaponItem->getTextureIndex();
			retval->attackGain = weaponItem->getAttackGain();
			retval->slotInBodyPartIndex = static_cast<int>(weaponItem->getSlotInBodyPart());
			return retval;
		}
    }
    return nullptr;
}

std::shared_ptr<Item> ManageWeaponItemController::itemDTOToItem(std::unique_ptr<ItemDTO> dto)
{
	WeaponItemDTO *weaponDTO = dynamic_cast<WeaponItemDTO *>(dto.get());
	WeaponItemCreationInfo creationInfo = {
        weaponDTO->id,
        weaponDTO->name,
        weaponDTO->textureName,
        weaponDTO->textureIndex,
		weaponDTO->attackGain,
		static_cast<WeaponBodyPart>(weaponDTO->slotInBodyPartIndex)
    };
    std::shared_ptr<Item> updateItem = nullptr;
    try {
        updateItem = std::make_shared<WeaponItem>(creationInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
    }
    return updateItem;
}
