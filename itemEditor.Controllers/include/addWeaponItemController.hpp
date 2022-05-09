#pragma once

#include "addItemController.hpp"
#include "itemStore.hpp"
#include "weaponItem.hpp"
#include <memory>
#include <string>

class AddWeaponItemController : public AddItemController
{
public:
    AddWeaponItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~AddWeaponItemController() = default;
    bool validateAttackGain(const std::string &attackGainStr); 
    bool addItem(const WeaponItemCreationInfo &itemInfo);
private:
    using AddItemController::addItem; //Hide the base class addItem method
};