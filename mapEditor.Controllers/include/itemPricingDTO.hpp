#pragma once

#include <optional>
#include <string>

namespace mapeditor::controllers {

    struct ItemPricingDTO {
      std::string itemId;
      unsigned int defaultBuyPrice;
      unsigned int defaultSellPrice;
      std::optional<unsigned int> buyPriceOverride;
      std::optional<unsigned int> sellPriceOverride;
    };

}  // namespace mapeditor::controllers
