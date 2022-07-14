#include "mapTileTriggerActionConverter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>

using namespace std;

string MapTileTriggerActionConverter::actionToString(MapTileTriggerAction action) 
{
    return allActionsToString()[static_cast<size_t>(action)];
}

boost::optional<MapTileTriggerAction> MapTileTriggerActionConverter::actionFromString(const std::string &actionStr) 
{
    auto allActionsStr { allActionsToString() };

    auto iter { find_if(allActionsStr.begin(), allActionsStr.end(), [&actionStr](const string &actionStr2) {
        return boost::iequals(actionStr, actionStr2);
        }) };
    if (iter != allActionsStr.end()) {
        return static_cast<MapTileTriggerAction>(iter - allActionsStr.begin());
    }
    return {};
}

vector<string> MapTileTriggerActionConverter::allActionsToString() 
{
    return { "None"s,
             "OpenChest"s,
             "ChangeMap"s,
             "DenyMove"s };
}