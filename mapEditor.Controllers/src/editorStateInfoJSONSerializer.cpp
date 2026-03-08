#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "editorStateInfoJSONSerializer.hpp"
#include "editorStateInfo.hpp"

using boost::property_tree::ptree;

namespace mapeditor::controllers {

std::string serializeEditorId(const EditorId id) {
    switch (id) {
        case EditorId::MapProperties:
            return "MapProperties";
        case EditorId::TextureSelection:
            return "TextureSelection";
        case EditorId::DebuggingInfo:
            return "DebuggingInfo";
    }
    throw std::invalid_argument("Unmanaged editor id");
}

EditorId deserializeEditorId(const std::string &id) {
    if (id == "MapProperties") {
        return EditorId::MapProperties;
    }
    if (id == "TextureSelection") {
        return EditorId::TextureSelection;
    }
    if (id == "DebuggingInfo") {
        return EditorId::DebuggingInfo;
    }
    throw std::invalid_argument("Unmanaged editor id");
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
    }
    if (location == "Left") {
        return DockLocation::Left;
    }
    if (location == "Right") {
        return DockLocation::Right;
    }
    throw std::invalid_argument("Unmanaged dock location");
}

ptree EditorStateInfoJSONSerializer::serialize(const std::vector<EditorStateInfo> &editorStates) {
    ptree retval;
    for (const auto &item : editorStates) {
        ptree editorStateNode;
        editorStateNode.put("id", serializeEditorId(item.id));
        editorStateNode.put("isVisible", item.isVisible);
        editorStateNode.put("dockLocation", serializeDockLocation(item.dockLocation));
        if (item.dockLocation != DockLocation::None) {
            editorStateNode.put("vIndex", item.vIndex);
        } else {
            editorStateNode.put("left", item.left);
            editorStateNode.put("top", item.top);
        }
        editorStateNode.put("width", item.width);
        editorStateNode.put("height", item.height);
        retval.push_back(std::make_pair("", editorStateNode));
    }
     return retval;
}

std::vector<EditorStateInfo> EditorStateInfoJSONSerializer::deserialize(ptree editorStateNode) {
    std::vector<EditorStateInfo> retval;
    for (const auto& editorState : editorStateNode.get_child("")) {
        retval.push_back({
            .id = deserializeEditorId(editorState.second.get<std::string>("id", "")),
            .isVisible = editorState.second.get<bool>("isVisible", true),
            .dockLocation = deserializeDockLocation(editorState.second.get<std::string>("dockLocation", "None")),
            .vIndex = editorState.second.get<int>("vIndex", 0),
            .left = editorState.second.get<int>("left", 0),
            .top = editorState.second.get<int>("top", 0),
            .width = editorState.second.get<int>("width", 0),
            .height = editorState.second.get<int>("height", 0),
        });
    }
    return retval;
}

}  // namespace mapeditor::controllers
