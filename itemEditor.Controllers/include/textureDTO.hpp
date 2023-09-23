#pragma once

#include <string>

namespace itemeditor::controllers {

struct TextureDTO
{
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};

} // namespace itemeditor::controllers
