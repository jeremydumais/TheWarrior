#pragma once

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

class MapTile
{
public:
    MapTile();
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    const std::string &getObjectTextureName() const;
    int getObjectTextureIndex() const;
    bool isAssigned() const;
    bool canPlayerSteppedOn() const;
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
    void setObjectTextureName(const std::string &name);
    void setObjectTextureIndex(int index);
    void setCanPlayerSteppedOn(bool value);
private:
    friend class boost::serialization::access;
    std::string textureName;
    int textureIndex;
    //Optional object on the tile
    std::string objectTextureName;
    int objectTextureIndex;
    bool canSteppedOn;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & textureName;
        ar & textureIndex;
        ar & objectTextureName;
        ar & objectTextureIndex;
        if(version > 0)
            ar & canSteppedOn;
    }
};
BOOST_CLASS_VERSION(MapTile, 1)