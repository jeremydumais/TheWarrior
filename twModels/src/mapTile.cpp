#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1),
      objectTextureName(""),
      objectTextureIndex(-1),
      canSteppedOn(true)
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

bool MapTile::isAssigned() const
{
    return (!textureName.empty() ||
            textureIndex != -1 || 
            !objectTextureName.empty() ||
            objectTextureIndex != -1);
}

bool MapTile::canPlayerSteppedOn() const
{
    return canSteppedOn;
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

void MapTile::setCanPlayerSteppedOn(bool value) 
{
    this->canSteppedOn = value;
}
