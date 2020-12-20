#pragma once

#include <string>

class MapTile
{
public:
    MapTile();
    const std::string &getTextureName() const;
    int getTextureIndex() const;
    void setTextureName(const std::string &name);
    void setTextureIndex(int index);
private:
    std::string textureName;
    int textureIndex;
};