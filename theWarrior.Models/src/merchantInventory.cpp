#include <fmt/format.h>
#include "merchantInventory.hpp"
#include <algorithm>
#include <map>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

using boost::algorithm::trim_copy;

namespace thewarrior::models {

void validateMerchantInventoryName(const std::string &name);

MerchantInventory::MerchantInventory(const std::string &name, MerchantInventoryType inventoryType)
: m_name(name),
  m_inventoryType(inventoryType) {
    validateMerchantInventoryName(name);
}

const std::string &MerchantInventory::getLastError() const {
    return m_lastError;
}

const std::string &MerchantInventory::getName() const {
    return m_name;
}

const MerchantInventoryType &MerchantInventory::getInventoryType() const {
    return m_inventoryType;
}

const std::vector<std::string> &MerchantInventory::getItemIds() const {
    return m_itemIds;
}

std::optional<ItemPriceOverride> MerchantInventory::getPriceOverridebyItemId(const std::string &itemId) const {
    auto iter = m_priceOverride.find(itemId);
    if (iter != m_priceOverride.end()) {
        return iter->second;
    }
    return std::nullopt;
}

bool MerchantInventory::setName(const std::string &name) {
    try {
        validateMerchantInventoryName(name);
        m_name = name;

    } catch (const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

bool MerchantInventory::addItem(const std::string &itemId) {
    auto iter = std::ranges::find(m_itemIds, itemId);
    if (iter != m_itemIds.end()) {
        m_lastError = fmt::format("The item {} already exist in the merchant inventory.", itemId);
        return false;
    }
    m_itemIds.push_back(itemId);
    return true;
}

bool MerchantInventory::removeItem(const std::string &itemId) {
    auto iter = std::ranges::find(m_itemIds, itemId);
    if (iter == m_itemIds.end()) {
        m_lastError = fmt::format("The item {} doesn'y exist in the merchant inventory.", itemId);
        return false;
    }
    m_itemIds.erase(iter);
    return true;
}

bool MerchantInventory::setPriceOverride(const std::string &itemId, const ItemPriceOverride &priceOverride) {
    auto iter = m_priceOverride.find(itemId);
    if (iter != m_priceOverride.end()) {
        m_lastError = fmt::format("The price override for the item {} already exist in the merchant inventory.", itemId);
        return false;
    }
    m_priceOverride.emplace(itemId, priceOverride);
    return true;
}

bool MerchantInventory::removePriceOverride(const std::string &itemId) {
    auto iter = m_priceOverride.find(itemId);
    if (iter == m_priceOverride.end()) {
        m_lastError = fmt::format("The price override for the item {} doesn't exist in the merchant inventory.", itemId);
        return false;
    }
    m_priceOverride.erase(iter);
    return true;
}

bool MerchantInventory::validate() {
    if (m_itemIds.empty()) {
        m_lastError = "The merchant inventory must contain at least one item.";
        return false;
    }
    return true;
}

void validateMerchantInventoryName(const std::string &name) {
    if (trim_copy(name).empty()) {
        throw std::invalid_argument("name cannot be empty.");
    }
}

}  // namespace thewarrior::models
