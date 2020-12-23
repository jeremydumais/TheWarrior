#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1),
      objectTextureName(""),
      objectTextureIndex(-1)
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

const std::string& MapTile::getObjectTextureName() const
{
    return objectTextureName;
}

int MapTile::getObjectTextureIndex() const
{
    return objectTextureIndex;
}

void MapTile::setTextureName(const std::string &name) 
{
    this->textureName = name;
}

void MapTile::setTextureIndex(int index) 
{
    this->textureIndex = index;
}

void MapTile::setObjectTextureName(const std::string &name) 
{
    this->objectTextureName = name;
}

void MapTile::setObjectTextureIndex(int index) 
{
    this->objectTextureIndex = index;
}