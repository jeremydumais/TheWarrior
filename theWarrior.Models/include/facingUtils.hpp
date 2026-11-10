#pragma once

#include "player.hpp"
#include "npc.hpp"

namespace thewarrior::models {

inline NPCFacing getNPCFacingOppositeOfPlayer(PlayerFacing playerFacing) {
    switch (playerFacing) {
        case PlayerFacing::Left:
            return NPCFacing::Right;
        case PlayerFacing::Right:
            return NPCFacing::Left;
        case PlayerFacing::Up:
            return NPCFacing::Down;
        case PlayerFacing::Down:
            return NPCFacing::Up;
    }

    return NPCFacing::Down;
}

}  // namespace thewarrior::models