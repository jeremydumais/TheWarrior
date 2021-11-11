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
    std::string m_name;
    std::string m_filename;
    int m_width;
    int m_height;
    int m_tileWidth;
    int m_tileHeight;
    float m_tileWidthGL;    
    float m_tileHeightGL;    
    void updateTileWidthGL();
    void updateTileHeightGL();
    Texture() = default; //Needed for deserialization
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_name;
        ar & m_filename;
        ar & m_width;
        ar & m_height;
        ar & m_tileWidth;
        ar & m_tileHeight;
        ar & m_tileWidthGL;
        ar & m_tileHeightGL;
    }
};
BOOST_CLASS_VERSION(Texture, 0)
