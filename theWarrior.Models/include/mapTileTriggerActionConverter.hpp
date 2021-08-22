#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

class MapTileTriggerActionConverter
{
public:
    static std::string actionToString(MapTileTriggerAction action);
    static boost::optional<MapTileTriggerAction> actionFromString(const std::string &actionStr);
    static std::vector<std::string> allActionsToString();
private:
    MapTileTriggerActionConverter() = delete;
};