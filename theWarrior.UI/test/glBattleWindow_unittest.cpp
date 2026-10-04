#include <gtest/gtest.h>
#include "glBattleWindow.hpp"
#include "battleAttackCalculation.hpp"
#include "statsItem.hpp"
#include "item.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui;

namespace {

class BattleWithoutRendering : public GLBattleWindow {
 public:
    explicit BattleWithoutRendering(std::shared_ptr<GLPlayer> player) {
        m_glPlayer = player;
        m_inputDevicesState = std::make_shared<InputDevicesState>();
    }
    void generateGLElements() override {}
    void finishItemTurn() { playerItemWorkflow(); }
    BattleAction action() const { return m_currentBattleAction; }
    void setDefeatState(DefeatMusicState state) {
        m_currentBattleAction = BattleAction::PlayerDied;
        m_defeatMusicState = state;
    }
    void finishDefeatStep() { playerDiedWorkflow(); }
    DefeatMusicState defeatState() const { return m_defeatMusicState; }
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

TEST(BattleDefeat, EarlyKeyPressDoesNotSkipMusicOrQueueConfirmation) {
    BattleWithoutRendering battle(std::make_shared<GLPlayer>("Warrior"));
    int confirmations = 0;
    battle.m_defeatConfirmed.connect([&]() { ++confirmations; });
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    for (auto state : {DefeatMusicState::FadingOut, DefeatMusicState::Playing}) {
        battle.setDefeatState(state);
        battle.processDefeatInput(event);
        EXPECT_EQ(state, battle.defeatState());
    }
    battle.setDefeatState(DefeatMusicState::WaitingForInput);
    EXPECT_EQ(0, confirmations);
    battle.processDefeatInput(event);
    EXPECT_EQ(1, confirmations);
    battle.processDefeatInput(event);
    EXPECT_EQ(1, confirmations);
}

TEST(BattleDefeat, RequiresFreshKeyPressAfterMusic) {
    BattleWithoutRendering battle(std::make_shared<GLPlayer>("Warrior"));
    battle.setDefeatState(DefeatMusicState::WaitingForInput);
    SDL_Event event{};
    event.type = SDL_KEYUP;
    battle.processDefeatInput(event);
    EXPECT_EQ(DefeatMusicState::WaitingForInput, battle.defeatState());
    event.type = SDL_KEYDOWN;
    event.key.repeat = 1;
    battle.processDefeatInput(event);
    EXPECT_EQ(DefeatMusicState::WaitingForInput, battle.defeatState());
    event.key.repeat = 0;
    battle.processDefeatInput(event);
    EXPECT_EQ(DefeatMusicState::Confirmed, battle.defeatState());
}

TEST(BattleDefeat, ControllerButtonCanConfirmDefeat) {
    BattleWithoutRendering battle(std::make_shared<GLPlayer>("Warrior"));
    battle.setDefeatState(DefeatMusicState::WaitingForInput);
    SDL_Event event{};
    event.type = SDL_CONTROLLERBUTTONDOWN;
    battle.processDefeatInput(event);
    EXPECT_EQ(DefeatMusicState::Confirmed, battle.defeatState());
}

TEST(BattleDefeat, MissingMusicStillRequiresConfirmationAndResetClearsState) {
    BattleWithoutRendering battle(std::make_shared<GLPlayer>("Warrior"));
    // The uninitialized test battle has no music, exercising the fallback path.
    battle.setDefeatState(DefeatMusicState::FadingOut);
    battle.finishDefeatStep();
    EXPECT_EQ(DefeatMusicState::WaitingForInput, battle.defeatState());
    battle.reset();
    EXPECT_FALSE(battle.isPlayerDefeated());
    EXPECT_EQ(DefeatMusicState::FadingOut, battle.defeatState());
}

TEST(BattleAttacks, WeakAttackStillHasAChanceToHit) {
    EXPECT_FLOAT_EQ(0.15F, battleHitChance(1.0F, 1000.0F));
    EXPECT_FLOAT_EQ(0.15F, battleHitChance(0.1F, 1000.0F));
    EXPECT_FLOAT_EQ(0.5F, battleHitChance(10.0F, 20.0F));
    EXPECT_GT(battleHitChance(10.0F, 20.0F), battleHitChance(5.0F, 20.0F));
    EXPECT_FLOAT_EQ(0.95F, battleHitChance(20.0F, 20.0F));
    EXPECT_FLOAT_EQ(0.95F, battleHitChance(100.0F, 20.0F));
    EXPECT_FLOAT_EQ(0.95F, battleHitChance(1.0F, 0.0F));
    EXPECT_FLOAT_EQ(0.0F, battleHitChance(0.0F, 20.0F));
}

TEST(BattleAttacks, LandedWeakHitsDealLimitedDamageForEitherSide) {
    for (float roll : {0.75F, 1.0F}) {
        EXPECT_EQ(1, battleHitDamage(1.0F, 100.0F, 1.0F, roll));
        EXPECT_EQ(1, battleHitDamage(1.0F, 100.0F / 2.0F, 1.0F, roll));
        EXPECT_EQ(1, battleHitDamage(0.1F, 100.0F, 2.0F, roll));
        EXPECT_EQ(0, battleHitDamage(0.0F, 100.0F, 1.0F, roll));
    }
}

TEST(BattleAttacks, StrongAttackDamageAndCriticalBonusArePreserved) {
    EXPECT_EQ(10, battleHitDamage(20.0F, 5.0F, 1.0F, 0.75F));
    EXPECT_EQ(15, battleHitDamage(20.0F, 5.0F, 1.0F, 1.0F));
    EXPECT_EQ(25, battleHitDamage(20.0F, 5.0F, 2.0F, 0.75F));
    EXPECT_EQ(18, battleHitDamage(20.0F, 5.0F / 2.0F, 1.0F, 1.0F));
}

}  // namespace
