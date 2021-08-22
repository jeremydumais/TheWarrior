#include "mapTileTriggerEventConverter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>

using namespace std;

string MapTileTriggerEventConverter::eventToString(MapTileTriggerEvent event) 
{
    return allEventsToString()[static_cast<size_t>(event)];
}

boost::optional<MapTileTriggerEvent> MapTileTriggerEventConverter::eventFromString(const std::string &eventStr) 
{
    auto allEventsStr { allEventsToString() };

    auto iter { find_if(allEventsStr.begin(), allEventsStr.end(), [&eventStr](const string &eventStr2) {
        return boost::iequals(eventStr, eventStr2);
        }) };
    if (iter != allEventsStr.end()) {
        return static_cast<MapTileTriggerEvent>(iter - allEventsStr.begin());
    }
    return {};
}

vector<string> MapTileTriggerEventConverter::allEventsToString() 
{
    return { "None"s,
             "SteppedOn"s,
             "MoveUpPressed"s,
             "MoveDownPressed"s,
             "MoveLeftPressed"s,
             "MoveRightPressed"s,
             "ActionButtonPressed"s };
}