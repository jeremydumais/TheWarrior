#pragma once

#include <string>
#include <map>

namespace mapeditor::controllers {

struct MapTileTriggerDTO {
    std::string event = "None";
    std::string condition = "None";
    std::string action = "None";
    std::map<std::string, std::string> actionProperties = {};
};

}  // namespace mapeditor::controllers
