#pragma once

#include <qlabel.h>
#include <qpixmap.h>
#include "point.hpp"
#include "texture.hpp"
#include "textureDTO.hpp"
#include "textureInfo.hpp"

namespace commoneditor::ui {

class TextureUtils {
 public:
    static int getTextureIndexFromPosition(const thewarrior::models::Point<> &pos,
                                           const thewarrior::models::Texture &texture,
                                           const float ratio = 1.0F);
    static QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap,
                                                  int tileIndex,
                                                  const thewarrior::models::Texture &texture);
    static QPixmap getTexturePixmapFromLabel(const QLabel *label);
    static thewarrior::models::TextureInfo TextureDTOToTextureInfo(const TextureDTO dto);
};

}  // namespace commoneditor::ui
