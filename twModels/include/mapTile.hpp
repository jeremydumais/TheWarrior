#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <map>
#include <string>
#include <vector>


enum class PlayerFacing
{
    Up,
    Down,
    Left,
    Right
};

class MapTile
{
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
    void addTrigger(const MapTileTrigger &trigger);
    bool updateTrigger(const MapTileTrigger &triggerToUpdate, const MapTileTrigger &updatedTrigger);
    bool deleteTrigger(const MapTileTrigger &triggerToDelete);
private:
    friend class boost::serialization::access;
    std::string textureName;
    int textureIndex;
    //Optional object on the tile
    std::string objectTextureName;
    int objectTextureIndex;
    bool canSteppedOn;
    bool objectAbovePlayer;
    bool isWallToClimb;
    std::vector<MapTileTrigger> triggers;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & textureName;
        ar & textureIndex;
        ar & objectTextureName;
        ar & objectTextureIndex;
        if(version > 0) {
            ar & canSteppedOn;
        }
        if(version > 1) {
            ar & objectAbovePlayer;
        }
        if(version >= 5) {
            ar & triggers;
        }
        if (version >= 6) {
            ar & isWallToClimb;
        } 
    }
};

BOOST_CLASS_VERSION(MapTile, 6)