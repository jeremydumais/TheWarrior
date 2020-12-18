#pragma once

#include <string>

class MapTile
{
public:
    MapTile();
    const std::string &getTextureName() const;
    int getTextureIndex() const;
private:
    std::string textureName;
    int textureIndex;
};