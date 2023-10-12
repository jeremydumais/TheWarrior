#include "../include/mapTile.hpp"
#include <algorithm>

namespace thewarrior::models {

MapTile::MapTile()
    : m_textureName(""),
      m_textureIndex(-1),
      m_objectTextureName(""),
      m_objectTextureIndex(-1),
      m_canSteppedOn(true),
      m_objectAbovePlayer(false),
      m_isWallToClimb(false),
      m_monsterZoneIndex(-1),
      m_triggers(std::vector<MapTileTrigger>()) {
}

const std::string& MapTile::getTextureName() const {
    return m_textureName;
}

int MapTile::getTextureIndex() const {
    return m_textureIndex;
}

bool MapTile::hasTexture() const {
    return !m_textureName.empty() && m_textureIndex != -1;
}

const std::string& MapTile::getObjectTextureName() const {
    return m_objectTextureName;
}

int MapTile::getObjectTextureIndex() const {
    return m_objectTextureIndex;
}

bool MapTile::hasObjectTexture() const {
    return !m_objectTextureName.empty() && m_objectTextureIndex != -1;
}

bool MapTile::isAssigned() const {
    return (!m_textureName.empty() ||
            m_textureIndex != -1 ||
            !m_objectTextureName.empty() ||
            m_objectTextureIndex != -1);
}

bool MapTile::canPlayerSteppedOn() const {
    return m_canSteppedOn;
}

bool MapTile::getObjectAbovePlayer() const {
    return m_objectAbovePlayer;
}

bool MapTile::getIsWallToClimb() const {
    return m_isWallToClimb;
}

int MapTile::getMonsterZoneIndex() const {
    return m_monsterZoneIndex;
}

const std::vector<MapTileTrigger> &MapTile::getTriggers() const {
    return m_triggers;
}

boost::optional<const MapTileTrigger &> MapTile::findConstTrigger(MapTileTriggerEvent event) const {
    auto iter = std::find_if(m_triggers.begin(), m_triggers.end(), [&event](const MapTileTrigger &trigger) {
            return trigger.getEvent() == event; });
    if (iter != m_triggers.end()) {
        return *iter;
    }
    return {};
}

boost::optional<MapTileTrigger &> MapTile::findTrigger(MapTileTriggerEvent event) {
    auto iter = std::find_if(m_triggers.begin(), m_triggers.end(), [&event](const MapTileTrigger &trigger) {
            return trigger.getEvent() == event; });
    if (iter != m_triggers.end()) {
        return *iter;
    }
    return {};
}

void MapTile::setTextureName(const std::string &name) {
    m_textureName = name;
}

void MapTile::setTextureIndex(int index) {
    m_textureIndex = index;
}

void MapTile::setObjectTextureName(const std::string &name) {
    m_objectTextureName = name;
}

void MapTile::setObjectTextureIndex(int index) {
    m_objectTextureIndex = index;
}

void MapTile::setCanPlayerSteppedOn(bool value) {
    m_canSteppedOn = value;
}

void MapTile::setObjectAbovePlayer(bool value) {
    m_objectAbovePlayer = value;
}

void MapTile::setIsWallToClimb(bool value) {
    m_isWallToClimb = value;
}

void MapTile::setMonsterZoneIndex(int index) {
    m_monsterZoneIndex = index;
}

void MapTile::addTrigger(const MapTileTrigger &trigger) {
    m_triggers.emplace_back(trigger);
}

bool MapTile::updateTrigger(const MapTileTrigger &triggerToUpdate, const MapTileTrigger &updatedTrigger) {
    auto iter { find_if(m_triggers.begin(), m_triggers.end(), [&triggerToUpdate](const MapTileTrigger &trigger) {
            return triggerToUpdate.getEvent() == trigger.getEvent();
            })};
    if (iter != m_triggers.end()) {
        *iter = updatedTrigger;
        return true;
    }
    return false;
}

bool MapTile::deleteTrigger(const MapTileTrigger &triggerToDelete) {
    auto oldSize { m_triggers.size() };
    auto newTriggersEnd { remove_if(m_triggers.begin(), m_triggers.end(), [&triggerToDelete](const MapTileTrigger &trigger) {
            return triggerToDelete.getEvent() == trigger.getEvent();
            })};
    m_triggers.erase(newTriggersEnd, m_triggers.end());
    return oldSize - m_triggers.size() > 0;
}

}  // namespace thewarrior::models
