#pragma once

#include "mapTileTrigger.hpp"
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
    const std::vector<MapTileTrigger> getTriggers() const;
    MapTileTriggerEvent getTrigger() const;
    MapTileTriggerCondition getCondition() const;
    MapTileTriggerAction getAction() const;
    const std::map<std::string, std::string> &getActionProperties() const;
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
    void setObjectTextureName(const std::string &name);
    void setObjectTextureIndex(int index);
    void setCanPlayerSteppedOn(bool value);
    void setObjectAbovePlayer(bool value);
    void setTrigger(MapTileTriggerEvent value);
    void setCondition(MapTileTriggerCondition value);
    void setAction(MapTileTriggerAction value);
    void setActionProperties(const std::map<std::string, std::string> &properties);
private:
    friend class boost::serialization::access;
    std::string textureName;
    int textureIndex;
    //Optional object on the tile
    std::string objectTextureName;
    int objectTextureIndex;
    bool canSteppedOn;
    bool objectAbovePlayer;
    std::vector<MapTileTrigger> triggers;
    MapTileTriggerEvent trigger;
    MapTileTriggerCondition condition;
    MapTileTriggerAction action;
    std::map<std::string, std::string> actionProperties;
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
        if(version > 2) {
            ar & trigger;
            ar & condition;
            ar & action;
            ar & actionProperties;
            if (trigger != MapTileTriggerEvent::None) {
                triggers.emplace_back(MapTileTrigger(trigger, condition, action, actionProperties));
            }
        }
        if(version > 4) {
            ar & triggers;
        }
    }
};
BOOST_CLASS_VERSION(MapTile, 4)