#pragma once

#include <optional>
#include <string>
#include "merchantInventoryDTO.hpp"
#include "merchantInventory.hpp"

namespace mapeditor::controllers {

class MerchantInventoryDTOUtils {
 public:
    struct MerchantInventoryConversionResult {
        std::optional<thewarrior::models::MerchantInventory> merchantInventory;
        std::string errorMessage;

        bool success() const noexcept { return merchantInventory.has_value(); }
    };
    static MerchantInventoryDTO fromMerchantInventory(const thewarrior::models::MerchantInventory &merchantInventory);
    static MerchantInventoryConversionResult toMerchantInventory(const MerchantInventoryDTO &dto);
};

}  // namespace mapeditor::controllers

