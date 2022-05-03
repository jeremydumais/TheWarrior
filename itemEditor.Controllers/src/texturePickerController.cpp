#include "texturePickerController.hpp"
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
    auto textures = m_textureContainer.getTextures();
    auto iter =  std::find_if(textures.begin(), 
                              textures.end(), 
                              [resourcesPath, textureName](const Texture &texture) { 
                                  return texture.getName() == textureName; 
                                  });
    if (iter != textures.end()) {
        if (!boost::algorithm::trim_copy(resourcesPath).empty()) {
            return fmt::format("{0}/{1}", resourcesPath, iter->getFilename());
        }
        else {
            return iter->getFilename();
        }
    }
    return "";
}


bool TexturePickerController::isTextureExist(const std::string &name) const
{
    auto textures = m_textureContainer.getTextures();
    return std::find_if(textures.begin(), 
                        textures.end(), 
                        [name](const Texture &texture) { return texture.getName() == name; }) != textures.end();
    
}

/*std::optional<std::reference_wrapper<const Texture>> TexturePickerController::getTextureByName(const std::string &name) const
{
    return m_textureContainer.getTextureByName(name);
}*/
