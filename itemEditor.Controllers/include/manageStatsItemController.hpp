#pragma once

#include "itemStore.hpp"
#include "manageItemController.hpp"
#include "statsItem.hpp"
#include <memory>
#include <string>

class ManageStatsItemController : public ManageItemController
{
public:
    ManageStatsItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageStatsItemController() = default;
    bool validateGain(const std::string &gainStr); 
    bool validateDurationInSecs(const std::string &duration);
    bool addItem(const StatsItemCreationInfo &itemInfo);
private:
    using ManageItemController::addItem; //Hide the base class addItem method
};