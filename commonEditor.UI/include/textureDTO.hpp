#pragma once

#include <string>

namespace commoneditor::ui {

struct TextureDTO {
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};

}  // namespace commoneditor::ui
