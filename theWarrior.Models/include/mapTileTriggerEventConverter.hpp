#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

class MapTileTriggerEventConverter
{
public:
    MapTileTriggerEventConverter() = delete;
    static std::string eventToString(MapTileTriggerEvent event);
    static boost::optional<MapTileTriggerEvent> eventFromString(const std::string &eventStr);
    static std::vector<std::string> allEventsToString();
};

} // namespace thewarrior::models
