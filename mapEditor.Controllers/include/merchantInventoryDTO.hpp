#pragma once

#include <optional>
#include <string>
#include <vector>
#include "merchantInventory.hpp"

namespace mapeditor::controllers {

struct MerchantInventoryItemDTO {
    std::string id;
    std::optional<unsigned int> buyPriceOverride;
    std::optional<unsigned int> sellPriceOverride;
};

struct MerchantInventoryDTO {
    std::string name;
    thewarrior::models::MerchantInventoryType inventoryType;
    std::vector<MerchantInventoryItemDTO> items;
};

}  // namespace mapeditor::controllers
