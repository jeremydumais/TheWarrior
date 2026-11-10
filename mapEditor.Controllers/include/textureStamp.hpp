#pragma once

#include "gameMap.hpp"
#include "texture.hpp"
#include <optional>
#include <string>
#include <vector>

namespace mapeditor::controllers {

// Indices are stored in atlas display order: left to right, top to bottom.
struct TextureStamp {
    std::string textureName;
    int width = 0;
    int height = 0;
    bool objectLayer = false;
    std::vector<int> indices;
};

TextureStamp makeTextureStamp(const thewarrior::models::Texture &texture,
                              int left, int top, int width, int height,
                              bool objectLayer);

std::optional<std::vector<thewarrior::models::MapTile>>
makeTextureStampPreview(const thewarrior::models::GameMap &map,
                       const TextureStamp &stamp,
                       thewarrior::models::Point<> origin);

} // namespace mapeditor::controllers
