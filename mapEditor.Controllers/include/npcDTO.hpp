#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "npc.hpp"
#include "point.hpp"

namespace mapeditor::controllers {

struct NPCDTO {
    std::string id;
    std::string name;
    std::string textureName;
    int baseTextureIndex = -1;
    thewarrior::models::Point<size_t> spawnPosition = thewarrior::models::Point<size_t>(0, 0);
    std::vector<thewarrior::models::Point<size_t>> wanderZone;
    std::vector<std::string> dialogueLines;
    thewarrior::models::NPCFacing defaultFacing = thewarrior::models::NPCFacing::Down;
    thewarrior::models::NPCFacing currentFacing = thewarrior::models::NPCFacing::Down;
    thewarrior::models::NPCBehavior defaultBehavior = thewarrior::models::NPCBehavior::Stationary;
    thewarrior::models::NPCBehavior currentBehavior = thewarrior::models::NPCBehavior::Stationary;  
};

}  // namespace mapeditor::controllers

