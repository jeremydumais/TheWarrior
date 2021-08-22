#pragma once

#include "textureInfo.hpp"
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

class Texture
{
public:
    explicit Texture(const TextureInfo &textureInfo);
    const std::string &getName() const;
    const std::string &getFilename() const;
    int getWidth() const;
    int getHeight() const;
    int getTileWidth() const;
    int getTileHeight() const;
    float getTileWidthGL() const;
    float getTileHeightGL() const;
    TextureInfo getTextureInfo() const;
    void setName(const std::string &name);
    void setFilename(const std::string &filename);
    void setWidth(int value);
    void setHeight(int value);
    void setTileWidth(int value);
    void setTileHeight(int value);  
private:
    friend class boost::serialization::access;
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    float tileWidthGL;    
    float tileHeightGL;    
    void updateTileWidthGL();
    void updateTileHeightGL();
    Texture() {}; //Needed for deserialization
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & name;
        ar & filename;
        ar & width;
        ar & height;
        ar & tileWidth;
        ar & tileHeight;
        ar & tileWidthGL;
        ar & tileHeightGL;
    }
};
BOOST_CLASS_VERSION(Texture, 0)
