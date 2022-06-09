#include "inventory.hpp"
#include "weaponItem.hpp"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>

ItemCreationInfo getItemCreationInfoSample()
{
    return {
        "key001",
        "Iron Key",
        "tex1",
        1
    };
}

std::shared_ptr<Item> getWeaponItemSample()
{
    std::shared_ptr<Item> retval(new WeaponItem({
        "swd001",
        "Iron Sword",
        "tex1",
        2,
        1.1F,
        WeaponBodyPart::MainHand
    }));
    return retval;
}

class InventoryWith3Samples : public ::testing::Test
{
public:
    InventoryWith3Samples()
    {
        for(int i = 1; i <= 3; i++) {
            auto info = getItemCreationInfoSample();
            info.id = fmt::format("key00{0}", i);
            inventory.addItem(std::make_shared<Item>(info));
        }
    }
    Inventory inventory;
};

class InventoryFull : public ::testing::Test
{
public:
    InventoryFull()
    {
        for(int i = 1; i <= INVENTORY_MAX; i++) {
            auto info = getItemCreationInfoSample();
            std::string idNumber = std::to_string(i);
            info.id = fmt::format("key0{0}", i<10 ? "0" + idNumber : idNumber);
            inventory.addItem(std::make_shared<Item>(info));
        }
    }
    Inventory inventory;
};

TEST(Inventory_DefaultConstructor, ReturnSuccess)
{
    Inventory inventory;
    for(const auto elem : inventory.getAllSlots()) {
        ASSERT_EQ(nullptr, elem);
    }
}

TEST(Inventory_GetAllSlots, ReturnInventoryMaxSlots)
{
    Inventory inventory;
    ASSERT_EQ(INVENTORY_MAX, inventory.getAllSlots().size());
}

TEST(Inventory_GetSlotCount, WithEmptyInventory_ReturnInventoryMax)
{
    Inventory inventory;
    ASSERT_EQ(INVENTORY_MAX, inventory.getSlotCount());
}

TEST_F(InventoryWith3Samples, GetItemsWithIndex_Return3Items)
{
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(3, actual.size());
    ASSERT_EQ("key001", actual[0]->getId());
    ASSERT_EQ("key002", actual[1]->getId());
    ASSERT_EQ("key003", actual[2]->getId());
}

TEST(Inventory_GetItemsWithIndex, WithEmptyInventory_Return0)
{
    Inventory inventory;
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(0, actual.size());
}

TEST(Inventory_GetItemCount, WithEmptyInventory_Return0)
{
    Inventory inventory;
    ASSERT_EQ(0, inventory.getItemCount());
}

TEST_F(InventoryWith3Samples, GetItemWithSlotIndex1_ReturnKey002) 
{
    auto actual = inventory.getItem(1);
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("key002", actual->getId());
}

TEST_F(InventoryWith3Samples, GetItemWithSlotIndex5_ReturnNull) 
{
    auto actual = inventory.getItem(5);
    ASSERT_EQ(nullptr, actual);
}

TEST_F(InventoryWith3Samples, GetItemWithOutOfBoundsSlotIndex36_ReturnNull) 
{
    auto actual = inventory.getItem(36);
    ASSERT_EQ(nullptr, actual);
}

TEST(Inventory_AddItem, WithNullItemOnEmptyInventory_ReturnFalse)
{
    Inventory inventory;
    ASSERT_FALSE(inventory.addItem(nullptr));
    ASSERT_EQ(0, inventory.getItemCount());
}

TEST(Inventory_AddItem, WithEmptyInventory_ReturnTrueAndInsertedInSlot0)
{
    Inventory inventory;
    ASSERT_TRUE(inventory.addItem(std::make_shared<Item>(getItemCreationInfoSample())));
    ASSERT_EQ(1, inventory.getItemCount());
    ASSERT_EQ("key001", inventory.getAllSlots()[0]->getId());
}

TEST_F(InventoryFull, AddItem_ReturnFalse)
{
    ASSERT_FALSE(inventory.addItem(std::make_shared<Item>(getItemCreationInfoSample())));
}

TEST(Inventory_DropItem, WithIndex0OnEmptyInventory_ReturnFalse)
{
    Inventory inventory;
    ASSERT_FALSE(inventory.dropItem(0));
}

TEST(Inventory_DropItem, WithOutOfBoundsIndex_ReturnFalse)
{
    Inventory inventory;
    ASSERT_FALSE(inventory.dropItem(36));
}

TEST_F(InventoryWith3Samples, DropItemWithIndex1_ReturnTrue)
{
    ASSERT_TRUE(inventory.dropItem(1));
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("key001", actual[0]->getId());
    ASSERT_EQ("key003", actual[2]->getId());
}

TEST_F(InventoryWith3Samples, MoveIndex1To2_ReturnTrue)
{
    ASSERT_TRUE(inventory.moveItem(1, 2));
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(3, actual.size());
    ASSERT_EQ("key003", actual[1]->getId());
    ASSERT_EQ("key002", actual[2]->getId());
}

TEST_F(InventoryWith3Samples, MoveIndex1To3_ReturnTrue)
{
    ASSERT_TRUE(inventory.moveItem(1, 3));
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(3, actual.size());
    ASSERT_EQ(nullptr, actual[1]);
    ASSERT_EQ("key002", actual[3]->getId());
}

TEST_F(InventoryWith3Samples, MoveIndex36To1_ReturnFalse)
{
    ASSERT_FALSE(inventory.moveItem(36, 1));
}

TEST_F(InventoryWith3Samples, MoveIndex1To36_ReturnFalse)
{
    ASSERT_FALSE(inventory.moveItem(1, 36));
}

TEST_F(InventoryFull, ReplaceItem_WithOutOfBoundSlotIndex_ReturnFalse)
{
    ASSERT_FALSE(inventory.replaceItem(36, getWeaponItemSample()));
}

TEST_F(InventoryFull, ReplaceItem_WithNullItem_ReturnFalse)
{
    ASSERT_FALSE(inventory.replaceItem(2, nullptr));
}

TEST_F(InventoryWith3Samples, ReplaceItem_WithItemAtAnUnusedSlot_ReturnFalse)
{
    ASSERT_FALSE(inventory.replaceItem(5, getWeaponItemSample()));
}

TEST_F(InventoryFull, ReplaceItem_WithWeaponAtIndex2_ReturnTrue)
{
    ASSERT_TRUE(inventory.replaceItem(1, getWeaponItemSample()));
    auto actual = inventory.getItemsWithIndex();
    ASSERT_EQ(36, actual.size());
    ASSERT_EQ("swd001", actual[1]->getId());
}