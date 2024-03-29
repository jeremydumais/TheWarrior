#pragma once

#include "armorItem.hpp"
#include "itemStore.hpp"
#include "manageItemController.hpp"
#include <memory>
#include <string>

namespace itemeditor::controllers {

struct ArmorItemDTO : public ItemDTO
{
    float defenseGain;
    int slotInBodyPartIndex;
};

class ManageArmorItemController : public ManageItemController
{
public:
    ManageArmorItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    virtual ~ManageArmorItemController() = default;
    bool validateDefenseGain(const std::string &defenseGainStr);
    virtual std::unique_ptr<ItemDTO> getItem(const std::string &id) const override;
protected:
    virtual std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto) override;
};

} // namespace itemeditor::controllers
