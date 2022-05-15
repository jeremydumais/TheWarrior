#pragma once

#include "itemStore.hpp"
#include "manageItemController.hpp"
#include "weaponItem.hpp"
#include <memory>
#include <string>

class ManageWeaponItemController : public ManageItemController
{
public:
    ManageWeaponItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageWeaponItemController() = default;
    bool validateAttackGain(const std::string &attackGainStr); 
    bool addItem(const WeaponItemCreationInfo &itemInfo);
private:
    using ManageItemController::addItem; //Hide the base class addItem method
};