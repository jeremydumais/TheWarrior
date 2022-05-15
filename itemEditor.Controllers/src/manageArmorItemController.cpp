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

bool ManageArmorItemController::addItem(const ArmorItemCreationInfo &itemInfo)
{
    std::shared_ptr<ArmorItem> newItem = nullptr;
    try {
        newItem = std::make_shared<ArmorItem>(itemInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }

    return addItemToStore(newItem);
}
