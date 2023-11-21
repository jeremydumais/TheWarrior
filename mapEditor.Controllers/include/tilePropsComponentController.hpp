#pragma once

#include <set>
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "glComponentController.hpp"
#include "mapTileDTO.hpp"
#include "mapTileTriggerDTO.hpp"

namespace mapeditor::controllers {

class TilePropsComponentController {
 public:
    explicit TilePropsComponentController(GLComponentController *glComponentController);
    const std::string &getLastError() const;
    std::vector<MapTileDTO> getSelectedTiles() const;
    std::set<MapTileTriggerDTO> getTilesCommonTriggers() const;
    boost::optional<MapTileTriggerDTO> findMapTileTriggerByEvent(const std::string &event);
    void setTilesTextureName(const std::string &name);
    void setTilesTextureIndex(int index);
    void setTilesObjectTextureName(const std::string &name);
    void setTilesObjectTextureIndex(int index);
    void setTilesObjectAbovePlayer(bool value);
    void setTilesCanSteppedOn(bool value);
    void setTilesIsWallToClimb(bool value);
    void setTilesMonsterZoneIndex(int index);
    bool addTilesTrigger(const MapTileTriggerDTO &triggerDTO);
 private:
    GLComponentController *m_glComponentController = nullptr;
    std::string m_lastError = "";
};

}  // namespace mapeditor::controllers

