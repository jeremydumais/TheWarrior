#pragma once

#include <item.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
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
    const std::shared_ptr<Item> getItem(size_t slotIndex) const;
    bool addItem(std::shared_ptr<Item> item);
    bool dropItem(size_t slotIndex);
    bool moveItem(size_t slotIndexSrc, size_t slotIndexDst);
    bool replaceItem(size_t slotIndexDst, std::shared_ptr<Item> item);
private:
    friend class boost::serialization::access;
    InventoryArray m_slots;
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
        //To define
    }
};
BOOST_CLASS_VERSION(Inventory, 0)