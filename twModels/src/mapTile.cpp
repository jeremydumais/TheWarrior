#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1),
      objectTextureName(""),
      objectTextureIndex(-1),
      canSteppedOn(true),
      objectAbovePlayer(false),
      triggers(std::vector<MapTileTrigger>())
      /*trigger(MapTileTriggerEvent::None),
      condition(MapTileTriggerCondition::None),
      action(MapTileTriggerAction::None)*/
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

const std::vector<MapTileTrigger> MapTile::getTriggers() const
{
    return triggers;
}

MapTileTriggerEvent MapTile::getTrigger() const
{
    return MapTileTriggerEvent::None;
    //TODO to Remove
    //return trigger;
}

MapTileTriggerCondition MapTile::getCondition() const
{
    return MapTileTriggerCondition::None;
    //TODO to Remove
    //return condition;
}

MapTileTriggerAction MapTile::getAction() const
{
    return MapTileTriggerAction::None;
    //TODO to Remove
    //return action;
}

const std::map<std::string, std::string> &MapTile::getActionProperties() const
{
    return std::map<std::string, std::string>();
    //TODO to Remove
    //return actionProperties;
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

void MapTile::setTrigger(MapTileTriggerEvent value) 
{
    //this->trigger = value;
    //TODO to Remove
}

void MapTile::setCondition(MapTileTriggerCondition value) 
{
    //this->condition = value;
    //TODO to Remove
}

void MapTile::setAction(MapTileTriggerAction value) 
{
    //this->action = value;
    //TODO to Remove
}

void MapTile::setActionProperties(const std::map<std::string, std::string> &properties) 
{
    //this->actionProperties = properties;
    //TODO to Remove
}
