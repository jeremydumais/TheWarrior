#pragma once

#include "point.hpp"
#include "textureContainer.hpp"
#include <qpixmap.h>
#include <optional>
#include <string>
#include <vector>

class TexturePickerController 
{
public:
    TexturePickerController(const TextureContainer &textureContainer);
    std::vector<std::string> getTextureNames() const;
    std::string getTextureFileName(const std::string &resourcesPath, const std::string &textureName) const;
    bool isTextureExist(const std::string &name) const;
    int getTextureIndexFromPosition(const Point &pos, const std::string &textureName) const;
    QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const std::string &textureName) const;
private:
    const TextureContainer &m_textureContainer;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;

};