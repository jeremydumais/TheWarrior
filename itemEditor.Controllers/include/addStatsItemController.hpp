#pragma once

#include "addItemController.hpp"
#include "itemStore.hpp"
#include "statsItem.hpp"
#include <memory>
#include <string>

class AddStatsItemController : public AddItemController
{
public:
    AddStatsItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~AddStatsItemController() = default;
    bool validateGain(const std::string &gainStr); 
    bool validateDurationInSecs(const std::string &duration);
    bool addItem(const StatsItemCreationInfo &itemInfo);
private:
    using AddItemController::addItem; //Hide the base class addItem method
};