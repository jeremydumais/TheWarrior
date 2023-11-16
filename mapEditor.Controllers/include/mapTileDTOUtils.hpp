#pragma once

#include <string>
#include <utility>
#include "mapTileDTO.hpp"
#include "mapTile.hpp"

namespace mapeditor::controllers {

class MapTileDTOUtils {
 public:
    static MapTileDTO fromMapTile(const thewarrior::models::MapTile &tile);
    static thewarrior::models::MapTile toMapTile(const MapTileDTO &dto);
};

}  // namespace mapeditor::controllers

