#pragma once

#include "armorItem.hpp"
#include "itemStore.hpp"
#include "manageItemController.hpp"
#include <memory>
#include <string>

class ManageArmorItemController : public ManageItemController
{
public:
    ManageArmorItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageArmorItemController() = default;
    bool validateDefenseGain(const std::string &defenseGainStr); 
    bool addItem(const ArmorItemCreationInfo &itemInfo);
private:
    using ManageItemController::addItem; //Hide the base class addItem method
};