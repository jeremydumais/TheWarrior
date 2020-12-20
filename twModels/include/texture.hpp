#pragma once

#include "textureInfo.hpp"
#include <string>

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
    void setName(const std::string &name);
    void setFilename(const std::string &filename);
    void setWidth(int value);
    void setHeight(int value);
    void setTileWidth(int value);
    void setTileHeight(int value);  
private:
    std::string name;
    std::string filename;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    
};
