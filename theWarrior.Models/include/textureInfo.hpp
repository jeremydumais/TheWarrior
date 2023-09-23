#pragma once

#include <string>

namespace thewarrior::models {

struct TextureInfo
{
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};

} // namespace thewarrior::models
