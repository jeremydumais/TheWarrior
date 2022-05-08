#include "addWeaponItemController.hpp"
#include "boost/algorithm/string.hpp"

AddWeaponItemController::AddWeaponItemController(std::shared_ptr<ItemStore> itemStore)
    : AddItemController(itemStore)
{
}

bool AddWeaponItemController::validateAttackGain(const std::string attackGainStr)
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

bool AddWeaponItemController::addItem(const WeaponItemCreationInfo &itemInfo)
{
    std::shared_ptr<WeaponItem> newItem = nullptr;
    try {
        newItem = std::make_shared<WeaponItem>(itemInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }

    return addItemToStore(newItem);
}
