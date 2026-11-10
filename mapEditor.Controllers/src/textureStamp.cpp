#include "textureStamp.hpp"

namespace mapeditor::controllers {

TextureStamp makeTextureStamp(const thewarrior::models::Texture &texture,
                              int left, int top, int width, int height,
                              bool objectLayer) {
    TextureStamp stamp{.textureName = texture.getName(),
                       .width = width,
                       .height = height,
                       .objectLayer = objectLayer,
                       .indices = {},};
    if (texture.getTileWidth() <= 0 || texture.getTileHeight() <= 0) {
        return {};
    }
    const int columns = texture.getWidth() / texture.getTileWidth();
    const int rows = texture.getHeight() / texture.getTileHeight();
    if (left < 0 || top < 0 || width <= 0 || height <= 0 ||
        width > columns - left || height > rows - top) {
        return {};
    }
    for (int y = top; y < top + height; ++y) {
        for (int x = left; x < left + width; ++x) {
            stamp.indices.push_back(((rows - 1 - y) * columns) + x);
        }
    }
    return stamp;
}

std::optional<std::vector<thewarrior::models::MapTile>>
makeTextureStampPreview(const thewarrior::models::GameMap &map,
                       const TextureStamp &stamp,
                       thewarrior::models::Point<> origin) {
    if (stamp.width <= 0 || stamp.height <= 0 || stamp.textureName.empty() ||
        stamp.indices.size() != static_cast<size_t>(stamp.width) *
                                    static_cast<size_t>(stamp.height) ||
        origin.x() < 0 || origin.y() < 0 ||
        stamp.width > static_cast<int>(map.getWidth()) - origin.x() ||
        stamp.height > static_cast<int>(map.getHeight()) - origin.y()) {
        return std::nullopt;
    }
    std::vector<thewarrior::models::MapTile> result;
    size_t source = 0;
    for (int y = 0; y < stamp.height; ++y) {
        for (int x = 0; x < stamp.width; ++x) {
            auto tile = map.getTileFromCoord({origin.x() + x, origin.y() + y});
            if (stamp.objectLayer) {
                tile.setObjectTextureName(stamp.textureName);
                tile.setObjectTextureIndex(stamp.indices[source++]);
            } else {
                tile.setTextureName(stamp.textureName);
                tile.setTextureIndex(stamp.indices[source++]);
            }
            result.push_back(tile);
        }
    }
    return result;
}

} // namespace mapeditor::controllers
