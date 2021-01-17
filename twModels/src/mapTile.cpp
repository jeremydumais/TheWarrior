#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1),
      objectTextureName(""),
      objectTextureIndex(-1),
      canSteppedOn(true),
      objectAbovePlayer(false),
      trigger(TileTrigger::None),
      condition(TileCondition::None),
      action(TileAction::None)
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

bool MapTile::hasTexture() const
{
    return !textureName.empty() && textureIndex != -1;
}

const std::string& MapTile::getObjectTextureName() const
{
    return objectTextureName;
}

int MapTile::getObjectTextureIndex() const
{
    return objectTextureIndex;
}

bool MapTile::hasObjectTexture() const
{
    return !objectTextureName.empty() && objectTextureIndex != -1;
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

bool MapTile::getObjectAbovePlayer() const
{
    return objectAbovePlayer;
}

TileTrigger MapTile::getTrigger() const
{
    return trigger;
}

TileCondition MapTile::getCondition() const
{
    return condition;
}

TileAction MapTile::getAction() const
{
    return action;
}

const std::map<std::string, std::string> &MapTile::getActionProperties() const
{
    return actionProperties;
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

void MapTile::setObjectAbovePlayer(bool value) 
{
    this->objectAbovePlayer = value;
}

void MapTile::setTrigger(TileTrigger value) 
{
    this->trigger = value;
}

void MapTile::setCondition(TileCondition value) 
{
    this->condition = value;
}

void MapTile::setAction(TileAction value) 
{
    this->action = value;
}

void MapTile::setActionProperties(const std::map<std::string, std::string> &properties) 
{
    this->actionProperties = properties;
}
