#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

class MapTileTriggerActionConverter
{
public:
    MapTileTriggerActionConverter() = delete;
    static std::string actionToString(MapTileTriggerAction action);
    static boost::optional<MapTileTriggerAction> actionFromString(const std::string &actionStr);
    static std::vector<std::string> allActionsToString();
};

} // namespace thewarrior::models
