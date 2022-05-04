#pragma once

#include "point.hpp"
#include "texture.hpp"
#include <qpixmap.h>

class TextureUtils
{
public:
    static int getTextureIndexFromPosition(const Point &pos, const Texture &texture);
    static QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const Texture &texture);
};