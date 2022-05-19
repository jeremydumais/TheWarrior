#include "manageArmorItemController.hpp"
#include "boost/algorithm/string.hpp"

ManageArmorItemController::ManageArmorItemController(std::shared_ptr<ItemStore> itemStore)
    : ManageItemController(itemStore)
{
}

bool ManageArmorItemController::validateDefenseGain(const std::string &defenseGainStr)
{
	if (boost::trim_copy(defenseGainStr).empty()) {
        m_lastError = "The defense gain value cannot be empty.";
		return false;
	}
	//Perform the str to float conversion
	try {
		std::stof(defenseGainStr);
	}
	catch(const std::invalid_argument &err) {
		m_lastError = "Unable to perform the float conversion of the defense gain.";
		return false;
	}
	catch(const std::out_of_range &err) {
		m_lastError = "The defense gain value is out of range.";
		return false;
	}
	return true;
}

std::unique_ptr<ItemDTO> ManageArmorItemController::getItem(const std::string &id) const 
{
	auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
		auto armorItem = dynamic_cast<const ArmorItem *>(item.get());
		if (armorItem != nullptr) {
			auto retval = std::make_unique<ArmorItemDTO>();
			retval->id = armorItem->getId();
			retval->name = armorItem->getName();
			retval->textureName = armorItem->getTextureName();
			retval->textureIndex = armorItem->getTextureIndex();
			retval->defenseGain = armorItem->getDefenseGain();
			retval->slotInBodyPartIndex = static_cast<int>(armorItem->getSlotInBodyPart());
			return retval;
		}
    }
    return nullptr;
}

std::shared_ptr<Item> ManageArmorItemController::itemDTOToItem(std::unique_ptr<ItemDTO> dto)
{
	ArmorItemDTO *armorDTO = dynamic_cast<ArmorItemDTO *>(dto.get());
	ArmorItemCreationInfo creationInfo = {
        armorDTO->id,
        armorDTO->name,
        armorDTO->textureName,
        armorDTO->textureIndex,
		armorDTO->defenseGain,
		static_cast<ArmorBodyPart>(armorDTO->slotInBodyPartIndex)
    };
    std::shared_ptr<Item> updateItem = nullptr;
    try {
        updateItem = std::make_shared<ArmorItem>(creationInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
    }
    return updateItem;
}
