#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

class MapTileTriggerEventConverter
{
public:
    static std::string eventToString(MapTileTriggerEvent event);
    static boost::optional<MapTileTriggerEvent> eventFromString(const std::string &eventStr);
    static std::vector<std::string> allEventsToString();
private:
    MapTileTriggerEventConverter() = delete;
};