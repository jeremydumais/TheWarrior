#pragma once

#include <string>
#include <utility>
#include "mapTileTriggerDTO.hpp"
#include "mapTileTrigger.hpp"

namespace mapeditor::controllers {

class MapTileTriggerDTOUtils {
 public:
    static MapTileTriggerDTO fromMapTileTrigger(const thewarrior::models::MapTileTrigger &trigger);
    static thewarrior::models::MapTileTrigger toMapTileTrigger(const MapTileTriggerDTO &dto);
};

}  // namespace mapeditor::controllers

