#pragma once

#include "point.hpp"
#include "textureContainer.hpp"
#include <qpixmap.h>
#include <optional>
#include <string>
#include <vector>

namespace itemeditor::controllers {

class TexturePickerController
{
public:
    TexturePickerController(const thewarrior::models::TextureContainer &textureContainer);
    std::vector<std::string> getTextureNames() const;
    std::string getTextureFileName(const std::string &resourcesPath, const std::string &textureName) const;
    bool isTextureExist(const std::string &name) const;
    int getTextureIndexFromPosition(const thewarrior::models::Point<> &pos, const std::string &textureName) const;
    QPixmap getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const std::string &textureName) const;
private:
    const thewarrior::models::TextureContainer &m_textureContainer;
    std::optional<std::reference_wrapper<const thewarrior::models::Texture>> getTextureByName(const std::string &name) const;
};

} // namespace itemeditor::controllers
