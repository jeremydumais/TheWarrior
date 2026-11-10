#include "encounterCooldown.hpp"
#include <gtest/gtest.h>

using thewarrior::ui::models::EncounterCooldown;

TEST(EncounterCooldown, DefaultSkipsRollsUntilFifthCompletedMove) {
    EncounterCooldown cooldown;
    EXPECT_FALSE(cooldown.canCheckEncounter());
    for (int move = 1; move < 5; ++move) {
        EXPECT_FALSE(cooldown.onTileMoveCompleted());
        EXPECT_FALSE(cooldown.canCheckEncounter());
    }
    EXPECT_TRUE(cooldown.onTileMoveCompleted());
    EXPECT_TRUE(cooldown.canCheckEncounter());
    for (int move = 0; move < 20; ++move) {
        EXPECT_TRUE(cooldown.onTileMoveCompleted());
    }
}

TEST(EncounterCooldown, ResetRestartsConfiguredMinimumAfterBattleOrMapChange) {
    EncounterCooldown cooldown(4);
    for (int move = 0; move < 4; ++move) {
        cooldown.onTileMoveCompleted();
    }
    ASSERT_TRUE(cooldown.canCheckEncounter());
    cooldown.reset();
    for (int move = 1; move < 4; ++move) {
        EXPECT_FALSE(cooldown.onTileMoveCompleted());
    }
    EXPECT_TRUE(cooldown.onTileMoveCompleted());
    cooldown.reset();
    EXPECT_FALSE(cooldown.canCheckEncounter());
}

TEST(EncounterCooldown, NoCompletedMovementPreservesProgress) {
    EncounterCooldown cooldown(2);
    EXPECT_FALSE(cooldown.onTileMoveCompleted());
    // Frames spent in menus, dialogue, or combat do not report tile moves.
    for (int frame = 0; frame < 100; ++frame) {
        EXPECT_FALSE(cooldown.canCheckEncounter());
    }
    EXPECT_TRUE(cooldown.onTileMoveCompleted());
}

TEST(EncounterCooldown, ZeroAllowsEveryEncounterCheck) {
    EncounterCooldown cooldown(0);
    EXPECT_TRUE(cooldown.canCheckEncounter());
    EXPECT_TRUE(cooldown.onTileMoveCompleted());
    cooldown.reset();
    EXPECT_TRUE(cooldown.canCheckEncounter());
}
