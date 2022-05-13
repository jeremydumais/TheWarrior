#include "addStatsItemController.hpp"
#include "boost/algorithm/string.hpp"
#include <limits>

AddStatsItemController::AddStatsItemController(std::shared_ptr<ItemStore> itemStore)
    : AddItemController(itemStore)
{
}

bool AddStatsItemController::validateGain(const std::string &gainStr)
{
	if (boost::trim_copy(gainStr).empty()) {
        m_lastError = "The gain value cannot be empty.";
		return false;
	}
	//Perform the str to float conversion
	try {
		std::stof(gainStr);
	}
	catch(const std::invalid_argument &err) {
		m_lastError = "Unable to perform the float conversion of the gain.";
		return false;
	}
	catch(const std::out_of_range &err) {
		m_lastError = "The gain value is out of range.";
		return false;
	}
	return true;
}

bool AddStatsItemController::validateDurationInSecs(const std::string &duration)
{
	if (boost::trim_copy(duration).empty()) {
        m_lastError = "The duration value cannot be empty.";
		return false;
	}
	//Perform the str to int conversion
	unsigned long durationUL = 0;
	try {
		durationUL = std::stoul(duration);
	}
	catch(const std::invalid_argument &err) {
		m_lastError = "Unable to perform the unsigned int conversion of the duration.";
		return false;
	}
	catch(const std::out_of_range &err) {
		m_lastError = "The duration value is out of range.";
		return false;
	}
	if (durationUL < std::numeric_limits<unsigned int>::min() ||
	    durationUL > std::numeric_limits<unsigned int>::max()) {
		m_lastError = "The duration value is out of range.";
		return false;
	}
	return true;
}
 
bool AddStatsItemController::addItem(const StatsItemCreationInfo &itemInfo)
{
    std::shared_ptr<StatsItem> newItem = nullptr;
    try {
        newItem = std::make_shared<StatsItem>(itemInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }

    return addItemToStore(newItem);
}
