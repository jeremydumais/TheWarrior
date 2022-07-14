#include "texturePickerController.hpp"
#include "textureUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <algorithm>

TexturePickerController::TexturePickerController(const TextureContainer &textureContainer)
    : m_textureContainer(textureContainer)
{
}

std::vector<std::string> TexturePickerController::getTextureNames() const
{
    std::vector<std::string> retval {};
    auto textures = m_textureContainer.getTextures();
    std::transform(textures.begin(), 
                   textures.end(), 
                   std::back_inserter(retval), 
                   [](const Texture &texture) -> std::string { return texture.getName(); });
    return retval;
}

std::string TexturePickerController::getTextureFileName(const std::string &resourcesPath, const std::string &textureName) const
{
    auto texture = getTextureByName(textureName);
    if (!texture.has_value()) {
        return "";
    }

    if (!boost::algorithm::trim_copy(resourcesPath).empty()) {
        return fmt::format("{0}/textures/{1}", resourcesPath, texture->get().getFilename());
    }
    else {
        return texture->get().getFilename();
    }
}

bool TexturePickerController::isTextureExist(const std::string &name) const
{
    return getTextureByName(name).has_value();    
}
    
int TexturePickerController::getTextureIndexFromPosition(const Point<> &pos, const std::string &textureName) const
{
    auto texture = getTextureByName(textureName);
    if (!texture.has_value()) {
        return -1;
    }
    return TextureUtils::getTextureIndexFromPosition(pos, texture->get());
}

QPixmap TexturePickerController::getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const std::string &textureName) const
{
    auto texture = getTextureByName(textureName);
    if (!texture.has_value()) {
        return QPixmap();
    }
    else {
        return TextureUtils::getTextureTileImageFromTexture(sourcePixmap, tileIndex, texture->get());
    }
}

std::optional<std::reference_wrapper<const Texture>> TexturePickerController::getTextureByName(const std::string &name) const
{
    auto textures = m_textureContainer.getTextures();
    auto iter =  std::find_if(textures.begin(), 
                              textures.end(), 
                              [name](const Texture &texture) { 
                                  return texture.getName() == name; 
                                  });
    if (iter == textures.end()) {
        return std::nullopt;
    }
    return {*iter};
}