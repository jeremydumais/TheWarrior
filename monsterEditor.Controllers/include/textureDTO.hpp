#pragma once

#include <string>

namespace MonsterEditorControllers
{
struct TextureDTO
{
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};
} // namespace MonsterEditorControllers
