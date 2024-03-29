#pragma once

#include "itemStore.hpp"
#include "manageItemController.hpp"
#include "weaponItem.hpp"
#include <memory>
#include <string>

namespace itemeditor::controllers {

struct WeaponItemDTO : public ItemDTO
{
    float attackGain;
    int slotInBodyPartIndex;
};

class ManageWeaponItemController : public ManageItemController
{
public:
    ManageWeaponItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    virtual ~ManageWeaponItemController() = default;
    bool validateAttackGain(const std::string &attackGainStr);
    virtual std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;
private:
    virtual std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};

} // namespace itemeditor::controllers
