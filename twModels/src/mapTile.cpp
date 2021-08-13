#include "mapTile.hpp"

MapTile::MapTile() 
    : textureName(""),
      textureIndex(-1),
      objectTextureName(""),
      objectTextureIndex(-1),
      canSteppedOn(true),
      objectAbovePlayer(false),
      triggers(std::vector<MapTileTrigger>())
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

const std::vector<MapTileTrigger> &MapTile::getTriggers() const
{
    return triggers;
}

boost::optional<const MapTileTrigger &> MapTile::findConstTrigger(MapTileTriggerEvent event) const
{
    auto iter = std::find_if(triggers.begin(), triggers.end(), [&event](const MapTileTrigger &trigger) {
	    return trigger.getEvent() == event; });
    if (iter != triggers.end()) {
        return *iter;
    }
    return {};
}

boost::optional<MapTileTrigger &> MapTile::findTrigger(MapTileTriggerEvent event) 
{
    auto iter = std::find_if(triggers.begin(), triggers.end(), [&event](const MapTileTrigger &trigger) {
	    return trigger.getEvent() == event; });
    if (iter != triggers.end()) {
        return *iter;
    }
    return {};
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

void MapTile::addTrigger(const MapTileTrigger &trigger) 
{
    triggers.emplace_back(trigger);
}
