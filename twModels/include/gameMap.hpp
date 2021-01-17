#pragma once

#include "mapTile.hpp"
#include "texture.hpp"
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

class GameMap
{
public:
    GameMap(unsigned int width, unsigned int height);
    const std::string getLastError() const;
    const std::vector<std::vector<MapTile>> &getTiles() const;
    MapTile &getTileForEditing(int index);
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    const std::vector<Texture> &getTextures() const;
    boost::optional<const Texture &> getTextureByName(const std::string &name) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    bool isShrinkMapImpactAssignedTiles(int offsetLeft, 
                                        int offsetTop, 
                                        int offsetRight, 
                                        int offsetBottom) const;
    void resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom);
    bool canSteppedOnTile(int playerX, int playerY);
private:
    friend class boost::serialization::access;
    std::string lastError;
    std::vector<std::vector<MapTile>> tiles;
    std::vector<Texture> textures;
    std::vector<Texture>::iterator _getTextureIterator(const std::string &name);
    bool _isShrinkMapFromLeftImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromTopImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromRightImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromBottomImpactAssignedTiles(int offset) const;
    void _resizeMapFromLeft(int offset);
    void _resizeMapFromTop(int offset);
    void _resizeMapFromRight(int offset);
    void _resizeMapFromBottom(int offset);
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        /*if(version > 0)
            ar & lastError;*/
        ar & tiles;
        ar & textures;
    }
};

BOOST_CLASS_VERSION(GameMap, 0)