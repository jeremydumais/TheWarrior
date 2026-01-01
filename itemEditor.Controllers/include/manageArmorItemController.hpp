#pragma once

#include <memory>
#include <string>
#include "itemStore.hpp"
#include "manageItemController.hpp"

namespace itemeditor::controllers {

struct ArmorItemDTO : public ItemDTO {
    float defenseGain;
    int slotInBodyPartIndex;
};

class ManageArmorItemController : public ManageItemController {
 public:
    explicit ManageArmorItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    ~ManageArmorItemController() override = default;
    bool validateDefenseGain(const std::string &defenseGainStr);
    std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;

 protected:
    std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};

}  // namespace itemeditor::controllers
