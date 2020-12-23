#pragma once

#include <string>

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
    std::string textureName;
    int textureIndex;
    //Optional object on the tile
    std::string objectTextureName;
    int objectTextureIndex;
};