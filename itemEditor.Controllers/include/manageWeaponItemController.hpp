#pragma once

#include <memory>
#include <string>
#include "itemStore.hpp"
#include "manageItemController.hpp"

namespace itemeditor::controllers {

struct WeaponItemDTO : public ItemDTO {
    float attackGain;
    int slotInBodyPartIndex;
};

class ManageWeaponItemController : public ManageItemController {
 public:
    explicit ManageWeaponItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    ~ManageWeaponItemController() override = default;
    bool validateAttackGain(const std::string &attackGainStr);
    std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;

 private:
    std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};

}  // namespace itemeditor::controllers
