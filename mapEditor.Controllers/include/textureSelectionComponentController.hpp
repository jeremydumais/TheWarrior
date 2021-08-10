#pragma once

#include "point.hpp"
#include "texture.hpp"

class TextureSelectionComponentController
{
public:
    int getTextureIndexFromPosition(const Point &pos, const Texture &texture);
};