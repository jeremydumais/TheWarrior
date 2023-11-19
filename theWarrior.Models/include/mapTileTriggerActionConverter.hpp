#pragma once

#include <string>
#include <vector>
#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>

namespace thewarrior::models {

class MapTileTriggerActionConverter {
 public:
    MapTileTriggerActionConverter() = delete;
    static std::string actionToString(MapTileTriggerAction action);
    static boost::optional<MapTileTriggerAction> actionFromString(const std::string &actionStr);
    static std::vector<std::string> allActionsToString();
};

}  // namespace thewarrior::models
