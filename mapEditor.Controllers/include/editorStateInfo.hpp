#pragma once

namespace mapeditor::controllers {

enum class EditorId {
    MapProperties,
    TextureSelection,
    DebuggingInfo
};

enum class DockLocation {
    None,
    Left,
    Right
};

struct EditorStateInfo {
    EditorId id;
    bool isVisible;
    DockLocation dockLocation;
    int vIndex;
    int left;
    int top;
    int width;
    int height;
};

}  // namespace mapeditor::controllers

