#pragma once

#include "point.hpp"
#include "texture.hpp"
#include <qlabel.h>
#include <qpixmap.h>

namespace commoneditor::ui {

class TextureUtils
{
public:
    static int getTextureIndexFromPosition(const thewarrior::models::Point<> &pos,
                                           const thewarrior::models::Texture &texture,
                                           const float ratio = 1.0F);
    static QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap,
                                                  int tileIndex,
                                                  const thewarrior::models::Texture &texture);
    static QPixmap getTexturePixmapFromLabel(const QLabel *label);
};

} // namespace commoneditor::ui
