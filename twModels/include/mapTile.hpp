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
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
    void setObjectTextureName(const std::string &name);
    void setObjectTextureIndex(int index);
private:
    friend class boost::serialization::access;
    std::string textureName;
    int textureIndex;
    //Optional object on the tile
    std::string objectTextureName;
    int objectTextureIndex;
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        /*if(version > 0)
            ar & textureName;*/
        ar & textureName;
        ar & textureIndex;
        ar & objectTextureName;
        ar & objectTextureIndex;
    }
};
BOOST_CLASS_VERSION(MapTile, 0)