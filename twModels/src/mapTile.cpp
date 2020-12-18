#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1)
{
}

const std::string& MapTile::getTextureName() const
{
    return textureName;
}

int MapTile::getTextureIndex() const
{
    return textureIndex;
}
