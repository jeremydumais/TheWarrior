#include "textureUtils.hpp"
#include <cmath>
#include "texture.hpp"
#include "textureDTO.hpp"

using thewarrior::models::Point;
using thewarrior::models::Texture;
using thewarrior::models::TextureInfo;

namespace commoneditor::ui {

int TextureUtils::getTextureIndexFromPosition(const Point<> &pos,
        const Texture &texture,
        const float ratio) {
    float textureWidth = static_cast<float>(texture.getWidth()) * ratio;
    float textureHeight = static_cast<float>(texture.getHeight()) * ratio;
    float tileWidth = static_cast<float>(texture.getTileWidth()) * ratio;
    float tileHeight = static_cast<float>(texture.getTileHeight()) * ratio;
    float realY = textureHeight - static_cast<float>(pos.y());
    float indexX = static_cast<float>(pos.x()) / tileWidth;
    float indexY = std::floor(realY / tileHeight);
    float tileIndex = indexX + (indexY * (textureWidth / tileWidth));
    return static_cast<int>(tileIndex);
}

QPixmap TextureUtils::getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const Texture &texture) {
    int textureWidthInPixel { texture.getWidth() };
    int textureHeightInPixel { texture.getHeight() };
    int x { (tileIndex * texture.getTileWidth()) % textureWidthInPixel };
    int y { textureHeightInPixel - (((tileIndex * texture.getTileWidth()) / textureWidthInPixel) * texture.getTileHeight()) };
    QPixmap imagePart = sourcePixmap->copy(x,
            y - texture.getTileHeight(),
            texture.getTileWidth(),
            texture.getTileHeight());
    return imagePart;
}

QPixmap TextureUtils::getTexturePixmapFromLabel(const QLabel *label) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return label->pixmap(Qt::ReturnByValue);
#else
    return *label->pixmap();
#endif
}

TextureInfo TextureUtils::TextureDTOToTextureInfo(const TextureDTO dto) {
    return TextureInfo {
        dto.name,
        dto.filename,
        dto.width,
        dto.height,
        dto.tileWidth,
        dto.tileHeight
    };
}

}  // namespace commoneditor::ui
