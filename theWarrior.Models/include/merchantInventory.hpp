#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "item.hpp"

namespace thewarrior::models {

enum class MerchantInventoryType {
    WeaponsAndArmors,
    StatsItems
};

struct ItemPriceOverride {
    std::optional<unsigned int> buyPrice;
    std::optional<unsigned int> sellPrice;
 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar & buyPrice;
        ar & sellPrice;
    }
};

class MerchantInventory {
 public:
    MerchantInventory(const std::string &name, MerchantInventoryType inventoryType);
    const std::string &getLastError() const;
    const std::string &getName() const;
    const MerchantInventoryType &getInventoryType() const;
    const std::vector<std::string> &getItemIds() const;
    std::optional<ItemPriceOverride> getPriceOverridebyItemId(const std::string &itemId) const;

    bool setName(const std::string &name);
    bool addItem(const std::string &itemId);
    bool removeItem(const std::string &itemId);
    bool setPriceOverride(const std::string &itemId, const ItemPriceOverride &priceOverride);
    bool removePriceOverride(const std::string &itemId);
    bool validate();


 private:
    friend class boost::serialization::access;
    MerchantInventory() = default;  // Used only for Boost Serialization
    std::string m_lastError;
    std::string m_name;
    MerchantInventoryType m_inventoryType;
    std::vector<std::string> m_itemIds;
    std::map<std::string, ItemPriceOverride> m_priceOverride;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/) {
        ar & m_name;
        ar & m_inventoryType;
        ar & m_itemIds;
        ar & m_priceOverride;
    }
};

}  // namespace thewarrior::models
