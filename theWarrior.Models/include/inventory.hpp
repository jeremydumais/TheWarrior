#pragma once

#include <item.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <array>
#include <map>
#include <memory>

namespace thewarrior::models {

#define INVENTORY_MAX 35

typedef std::array<std::shared_ptr<const Item>, INVENTORY_MAX> InventoryArray;

class Inventory
{
public:
    Inventory();
    const InventoryArray &getAllSlots() const;
    size_t getSlotCount() const;
    std::map<size_t, std::shared_ptr<const Item>> getItemsWithIndex() const;
    size_t getItemCount() const;
    const std::shared_ptr<const Item> getItem(size_t slotIndex) const;
    bool addItem(std::shared_ptr<const Item> item);
    bool dropItem(size_t slotIndex);
    bool moveItem(size_t slotIndexSrc, size_t slotIndexDst);
    bool replaceItem(size_t slotIndexDst, std::shared_ptr<const Item> item);
private:
    friend class boost::serialization::access;
    InventoryArray m_slots;
    //Serialization method
    template<class Archive>
    void serialize(Archive &, const unsigned int) {
        //To define
    }
};

} // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::Inventory, 0)
