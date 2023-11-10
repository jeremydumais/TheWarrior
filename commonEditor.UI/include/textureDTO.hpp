#pragma once

#include <string>

namespace commoneditor::ui {

struct TextureDTO {
    std::string name = "";
    std::string filename = "";
    int width = 0;
    int height = 0;
    int tileWidth = 0;
    int tileHeight = 0;
};

}  // namespace commoneditor::ui
