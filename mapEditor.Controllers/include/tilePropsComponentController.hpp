#pragma once

#include <set>
#include <string>
#include <vector>
#include "glComponentController.hpp"
#include "mapTileDTO.hpp"
#include "mapTileTriggerDTO.hpp"

namespace mapeditor::controllers {

class TilePropsComponentController {
 public:
    explicit TilePropsComponentController(GLComponentController *glComponentController);
    std::vector<MapTileDTO> getSelectedTiles() const;
    std::set<MapTileTriggerDTO> getSelectedTilesCommonTriggers() const;
    void setSelectedTilesTextureName(const std::string &name);
    void setSelectedTilesTextureIndex(int index);
    void setSelectedTilesObjectTextureName(const std::string &name);
    void setSelectedTilesObjectTextureIndex(int index);
    void setSelectedTilesObjectAbovePlayer(bool value);
    void setSelectedTilesCanSteppedOn(bool value);
    void setSelectedTilesIsWallToClimb(bool value);
    void setSelectedTilesMonsterZoneIndex(int index);
    void addSelectedTilesTrigger(const MapTileTriggerDTO &trigger);
 private:
    GLComponentController *m_glComponentController = nullptr;
};

}  // namespace mapeditor::controllers

