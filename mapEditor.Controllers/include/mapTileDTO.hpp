#pragma once

#include <string>
#include <vector>

namespace mapeditor::controllers {

struct MapTileDTO {
    std::string textureName = "";
    int textureIndex = -1;
    std::string objectTextureName = "";
    int objectTextureIndex = -1;
    bool canSteppedOn = true;
    bool objectAbovePlayer = false;
    bool isWallToClimb = false;
    int monsterZoneIndex = -1;
    std::vector<std::string> triggers = {};
};

}  // namespace mapeditor::controllers


