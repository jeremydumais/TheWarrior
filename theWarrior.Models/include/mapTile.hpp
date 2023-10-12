#pragma once

#include <map>
#include <string>
#include <vector>
#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

class MapTile {
 public:
    MapTile();
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    bool hasTexture() const;
    const std::string &getObjectTextureName() const;
    int getObjectTextureIndex() const;
    bool hasObjectTexture() const;
    bool isAssigned() const;
    bool canPlayerSteppedOn() const;
    bool getObjectAbovePlayer() const;
    bool getIsWallToClimb() const;
    int getMonsterZoneIndex() const;
    const std::vector<MapTileTrigger> &getTriggers() const;
    boost::optional<const MapTileTrigger &> findConstTrigger(MapTileTriggerEvent event) const;
    boost::optional<MapTileTrigger &> findTrigger(MapTileTriggerEvent event);
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
    void setObjectTextureName(const std::string &name);
    void setObjectTextureIndex(int index);
    void setCanPlayerSteppedOn(bool value);
    void setObjectAbovePlayer(bool value);
    void setIsWallToClimb(bool value);
    void setMonsterZoneIndex(int index);
    void addTrigger(const MapTileTrigger &trigger);
    bool updateTrigger(const MapTileTrigger &triggerToUpdate, const MapTileTrigger &updatedTrigger);
    bool deleteTrigger(const MapTileTrigger &triggerToDelete);

 private:
    friend class boost::serialization::access;
    std::string m_textureName;
    int m_textureIndex;
    // Optional object on the tile
    std::string m_objectTextureName;
    int m_objectTextureIndex;
    bool m_canSteppedOn;
    bool m_objectAbovePlayer;
    bool m_isWallToClimb;
    int m_monsterZoneIndex;
    std::vector<MapTileTrigger> m_triggers;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_textureName;
        ar & m_textureIndex;
        ar & m_objectTextureName;
        ar & m_objectTextureIndex;
        if (version > 0) {
            ar & m_canSteppedOn;
        }
        if (version > 1) {
            ar & m_objectAbovePlayer;
        }
        if (version >= 5) {
            ar & m_triggers;
        }
        if (version >= 6) {
            ar & m_isWallToClimb;
        }
        if (version >= 7) {
            ar & m_monsterZoneIndex;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MapTile, 7)
