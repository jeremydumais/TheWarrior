#include <gtest/gtest.h>
#include "facingUtils.hpp"

using thewarrior::models::getNPCFacingOppositeOfPlayer;
using thewarrior::models::NPCFacing;
using thewarrior::models::PlayerFacing;

TEST(FacingUtils_getNPCFacingOppositeOfPlayer, PlayerFacingLeft_ReturnNPCFacingRight) {
    ASSERT_EQ(NPCFacing::Right, getNPCFacingOppositeOfPlayer(PlayerFacing::Left));
}

TEST(FacingUtils_getNPCFacingOppositeOfPlayer, PlayerFacingRight_ReturnNPCFacingLeft) {
    ASSERT_EQ(NPCFacing::Left, getNPCFacingOppositeOfPlayer(PlayerFacing::Right));
}

TEST(FacingUtils_getNPCFacingOppositeOfPlayer, PlayerFacingUp_ReturnNPCFacingDown) {
    ASSERT_EQ(NPCFacing::Down, getNPCFacingOppositeOfPlayer(PlayerFacing::Up));
}

TEST(FacingUtils_getNPCFacingOppositeOfPlayer, PlayerFacingDown_ReturnNPCFacingUp) {
    ASSERT_EQ(NPCFacing::Up, getNPCFacingOppositeOfPlayer(PlayerFacing::Down));
}
