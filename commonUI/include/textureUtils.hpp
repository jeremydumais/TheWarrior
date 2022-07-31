#pragma once

#include "point.hpp"
#include "texture.hpp"
#include <qlabel.h>
#include <qpixmap.h>

class TextureUtils
{
public:
    static int getTextureIndexFromPosition(const Point<> &pos,
                                           const Texture &texture,
                                           const float ratio = 1.0F);
    static QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const Texture &texture);
    static QPixmap getTexturePixmapFromLabel(const QLabel *label);
};
