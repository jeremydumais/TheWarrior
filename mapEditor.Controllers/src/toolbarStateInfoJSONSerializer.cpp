#include "toolbarStateInfoJSONSerializer.hpp"
#include <stdexcept>
#include "toolbarStateInfo.hpp"

using boost::property_tree::ptree;

namespace mapeditor::controllers {

std::string serializeToolBarId(const ToolBarId id) {
    switch (id) {
        case ToolBarId::MapProperties:
            return "MapProperties";
        case ToolBarId::TextureSelection:
            return "TextureSelection";
        case ToolBarId::DebuggingInfo:
            return "DebuggingInfo";
    }
    throw std::invalid_argument("Unmanaged toolbar id");
}

ToolBarId deserializeToolBarId(const std::string &id) {
    if (id == "MapProperties") {
        return ToolBarId::MapProperties;
    } else if (id == "TextureSelection") {
        return ToolBarId::TextureSelection;
    } else if (id == "DebuggingInfo") {
        return ToolBarId::DebuggingInfo;
    }
    throw std::invalid_argument("Unmanaged toolbar id");
}

std::string serializeDockLocation(const DockLocation location) {
    switch (location) {
        case DockLocation::None:
            return "None";
        case DockLocation::Left:
            return "Left";
        case DockLocation::Right:
            return "Right";
    }
    throw std::invalid_argument("Unmanaged dock location");
}

DockLocation deserializeDockLocation(const std::string &location) {
    if (location == "None") {
        return DockLocation::None;
    } else if (location == "Left") {
        return DockLocation::Left;
    } else if (location == "Right") {
        return DockLocation::Right;
    }
    throw std::invalid_argument("Unmanaged dock location");
}

ptree ToolBarStateInfoJSONSerializer::serialize(const std::vector<ToolBarStateInfo> &toolbarStates) {
    ptree retval;
    for (const auto &item : toolbarStates) {
        ptree toolbarStateNode;
        toolbarStateNode.put("id", serializeToolBarId(item.id));
        toolbarStateNode.put("isVisible", item.isVisible);
        toolbarStateNode.put("dockLocation", serializeDockLocation(item.dockLocation));
        if (item.dockLocation != DockLocation::None) {
            toolbarStateNode.put("vIndex", item.vIndex);
        } else {
            toolbarStateNode.put("left", item.left);
            toolbarStateNode.put("top", item.top);
        }
        toolbarStateNode.put("width", item.width);
        toolbarStateNode.put("height", item.height);
        retval.push_back(std::make_pair("", toolbarStateNode));
    }
     return retval;
}

std::vector<ToolBarStateInfo> ToolBarStateInfoJSONSerializer::deserialize(ptree toolbarStateNode) {
    std::vector<ToolBarStateInfo> retval;
    for (const auto& toolbarState : toolbarStateNode.get_child("")) {
        retval.push_back({
            .id = deserializeToolBarId(toolbarState.second.get<std::string>("id", "")),
            .isVisible = toolbarState.second.get<bool>("isVisible", true),
            .dockLocation = deserializeDockLocation(toolbarState.second.get<std::string>("dockLocation", "None")),
            .vIndex = toolbarState.second.get<int>("vIndex", 0),
            .left = toolbarState.second.get<int>("left", 0),
            .top = toolbarState.second.get<int>("top", 0),
            .width = toolbarState.second.get<int>("width", 0),
            .height = toolbarState.second.get<int>("height", 0),
        });
    }
    return retval;
}

}  // namespace mapeditor::controllers
