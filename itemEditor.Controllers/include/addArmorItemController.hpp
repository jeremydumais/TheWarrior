#pragma once

#include "addItemController.hpp"
#include "itemStore.hpp"
#include "armorItem.hpp"
#include <memory>
#include <string>

class AddArmorItemController : public AddItemController
{
public:
    AddArmorItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~AddArmorItemController() = default;
    bool validateDefenseGain(const std::string defenseGainStr); 
    bool addItem(const ArmorItemCreationInfo &itemInfo);
private:
    using AddItemController::addItem; //Hide the base class addItem method
};