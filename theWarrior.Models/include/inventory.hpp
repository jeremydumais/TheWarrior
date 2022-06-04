#pragma once

#include <item.hpp>
#include <array>
#include <map>
#include <memory>

#define INVENTORY_MAX 36

typedef std::array<std::shared_ptr<Item>, INVENTORY_MAX> InventoryArray;

class Inventory
{
public:
    Inventory();
    const InventoryArray &getAllSlots() const;
    size_t getSlotCount() const;
    std::map<size_t, std::shared_ptr<Item>> getItemsWithIndex() const;
    size_t getItemCount() const;
    bool addItem(std::shared_ptr<Item> item);
    bool dropItem(size_t slotIndex);
    bool moveItem(size_t slotIndexSrc, size_t slotIndexDst);
private:
    InventoryArray m_slots;
};