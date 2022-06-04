#include "inventory.hpp"
#include <algorithm>

Inventory::Inventory()
{
    std::fill(m_slots.begin(), m_slots.end(), nullptr);
}

const InventoryArray &Inventory::getAllSlots() const
{
    return m_slots;
}

size_t Inventory::getSlotCount() const
{
    return INVENTORY_MAX;
}

std::map<size_t, std::shared_ptr<Item>> Inventory::getItemsWithIndex() const
{
    std::map<size_t, std::shared_ptr<Item>> retval = {};
    size_t i = 0;
    for(auto &slot : m_slots) {
        if (slot != nullptr) {
            retval.insert({i, slot});
        }
        i++;
    }
    return retval;
}

size_t Inventory::getItemCount() const
{
    auto count = std::count_if(m_slots.begin(),
                               m_slots.end(),
                               [](const auto item) {
                                    return item != nullptr;
                               });
    return count < 0 ? 0 : static_cast<size_t>(count);
}

bool Inventory::addItem(std::shared_ptr<Item> item)
{
    if (item == nullptr) {
        return false;
    }
    //Check if there's an available slot
    size_t i = 0;
    for(auto &slot : m_slots) {
        if (slot == nullptr) {
            slot = item;
            return true;
        }
        i++;
    }
    return false;
}

bool Inventory::dropItem(size_t slotIndex)
{
    if (slotIndex >= INVENTORY_MAX) {
        return false;
    }
    if (m_slots.at(slotIndex) != nullptr) {
        m_slots.at(slotIndex).reset();
        return true;
    }
    return false;
}

bool Inventory::moveItem(size_t slotIndexSrc, size_t slotIndexDst)
{
    if (slotIndexSrc >= INVENTORY_MAX || slotIndexDst >= INVENTORY_MAX) {
        return false;
    }
    auto &srcSlot = m_slots.at(slotIndexSrc);
    if (srcSlot == nullptr) {
        return false;
    }
    auto &dstSlot = m_slots.at(slotIndexDst);
    std::shared_ptr<Item> temp = dstSlot;
    dstSlot = srcSlot;
    srcSlot = temp;
    return true;
}
