#include <gtest/gtest.h>
#include "glBattleWindow.hpp"
#include "statsItem.hpp"
#include "item.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui;

namespace {

class BattleWithoutRendering : public GLBattleWindow {
 public:
    explicit BattleWithoutRendering(std::shared_ptr<GLPlayer> player) {
        m_glPlayer = player;
    }
    void generateGLElements() override {}
    void finishItemTurn() { playerItemWorkflow(); }
    BattleAction action() const { return m_currentBattleAction; }
};

std::shared_ptr<StatsItem> potion(Stats stat = Stats::Vitality) {
    return std::make_shared<StatsItem>(StatsItemCreationInfo{
        "pot001", "Potion", "tex1", 0, "", 2, 1, stat, 10.0F, false, 0});
}

TEST(BattleItems, DefersHealingAndConsumptionUntilPlayerItemTurn) {
    auto player = std::make_shared<GLPlayer>("Warrior");
    player->reduceHealth(12);
    auto item = potion();
    ASSERT_TRUE(player->getInventory()->addItem(item));
    BattleWithoutRendering battle(player);

    ASSERT_TRUE(battle.useInventoryItem(0));
    EXPECT_EQ(BattleAction::PlayerItem, battle.action());
    EXPECT_EQ(3, player->getStats().health);
    EXPECT_EQ(item, player->getInventory()->getItem(0));
    EXPECT_FALSE(battle.useInventoryItem(0));

    battle.finishItemTurn();
    EXPECT_EQ(13, player->getStats().health);
    EXPECT_EQ(nullptr, player->getInventory()->getItem(0));
    EXPECT_EQ(BattleAction::MonsterTurn, battle.action());
    EXPECT_FALSE(battle.useInventoryItem(0));
}

TEST(BattleItems, HealingIsCappedAndOnlyOneItemIsConsumed) {
    auto player = std::make_shared<GLPlayer>("Warrior");
    player->reduceHealth(1);
    ASSERT_TRUE(player->getInventory()->addItem(potion()));
    auto second = potion();
    ASSERT_TRUE(player->getInventory()->addItem(second));
    BattleWithoutRendering battle(player);
    ASSERT_TRUE(battle.useInventoryItem(0));
    battle.finishItemTurn();
    EXPECT_EQ(player->getStats().maxHealth, player->getStats().health);
    EXPECT_EQ(second, player->getInventory()->getItem(1));
}

TEST(BattleItems, InvalidAndUnsupportedItemsDoNotSpendATurn) {
    auto player = std::make_shared<GLPlayer>("Warrior");
    ASSERT_TRUE(player->getInventory()->addItem(potion(Stats::Strength)));
    ASSERT_TRUE(player->getInventory()->addItem(std::make_shared<Item>()));
    BattleWithoutRendering battle(player);
    EXPECT_FALSE(battle.useInventoryItem(0));
    EXPECT_FALSE(battle.useInventoryItem(1));
    EXPECT_FALSE(battle.useInventoryItem(2));
    EXPECT_FALSE(battle.useInventoryItem(INVENTORY_MAX));
    EXPECT_EQ(BattleAction::PlayerTurn, battle.action());
    EXPECT_NE(nullptr, player->getInventory()->getItem(0));
}

TEST(BattleItems, RemovedItemReturnsToPlayerTurnWithoutHealing) {
    auto player = std::make_shared<GLPlayer>("Warrior");
    player->reduceHealth(12);
    ASSERT_TRUE(player->getInventory()->addItem(potion()));
    BattleWithoutRendering battle(player);
    ASSERT_TRUE(battle.useInventoryItem(0));
    player->getInventory()->dropItem(0);
    battle.finishItemTurn();
    EXPECT_EQ(3, player->getStats().health);
    EXPECT_EQ(BattleAction::PlayerTurn, battle.action());
}

TEST(BattleItems, ResetClearsPendingSelectionWithoutConsumingIt) {
    auto player = std::make_shared<GLPlayer>("Warrior");
    ASSERT_TRUE(player->getInventory()->addItem(potion()));
    BattleWithoutRendering battle(player);
    ASSERT_TRUE(battle.useInventoryItem(0));
    battle.reset();
    EXPECT_NE(nullptr, player->getInventory()->getItem(0));
    EXPECT_TRUE(battle.useInventoryItem(0));
}

}  // namespace
