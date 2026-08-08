#include <algorithm>
#include <optional>
#include <ranges>
#include <stdexcept>
#include "merchantInventoryDTOUtils.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"

using mapeditor::controllers::MerchantInventoryDTO;
using mapeditor::controllers::MerchantInventoryItemDTO;
using thewarrior::models::ItemPriceOverride;
using thewarrior::models::MerchantInventory;

namespace mapeditor::controllers
{

    MerchantInventoryDTO MerchantInventoryDTOUtils::fromMerchantInventory(const MerchantInventory &merchantInventory) {
        std::vector<MerchantInventoryItemDTO> items;
        std::ranges::for_each(merchantInventory.getItemIds(),
                              [&items, &merchantInventory](const auto &itemId) {
                                MerchantInventoryItemDTO itemDTO;
                                itemDTO.id = itemId;
                                const std::optional<ItemPriceOverride> &priceOverride = merchantInventory.getPriceOverridebyItemId(itemId);
                                if (priceOverride.has_value()) {
                                    itemDTO.buyPriceOverride = priceOverride->buyPrice;
                                    itemDTO.sellPriceOverride = priceOverride->sellPrice;
                                }
                                items.push_back(itemDTO);
                              });
        return MerchantInventoryDTO{
            .name = merchantInventory.getName(),
            .inventoryType = merchantInventory.getInventoryType(),
            .items = items
        };
    }

    MerchantInventoryDTOUtils::MerchantInventoryConversionResult MerchantInventoryDTOUtils::toMerchantInventory(const MerchantInventoryDTO &dto) {
        try {
            MerchantInventory result(dto.name, dto.inventoryType);
            for (const auto &item : dto.items) {
                if (!result.addItem(item.id)) {
                    return {.merchantInventory = std::nullopt,
                            .errorMessage = result.getLastError()};
                }
                if (item.buyPriceOverride.has_value() || item.sellPriceOverride.has_value()) {
                    const ItemPriceOverride priceOverride = {
                        .buyPrice = item.buyPriceOverride,
                        .sellPrice = item.sellPriceOverride
                    };
                    if (!result.setPriceOverride(item.id, priceOverride)) {
                        return {.merchantInventory = std::nullopt,
                                .errorMessage = result.getLastError()};
                    }
                }
            }
            return {.merchantInventory = result, .errorMessage = ""};
        }
        catch (const std::invalid_argument &err) {
            return {.merchantInventory = std::nullopt, .errorMessage = err.what()};
        }
    }

}  // namespace mapeditor::controllers
