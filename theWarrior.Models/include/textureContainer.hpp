#pragma once

#include "texture.hpp"
#include <vector>

class TextureContainer
{
public:
    TextureContainer();
    size_t getCount() const;
private:
    std::vector<Texture> textures;
};