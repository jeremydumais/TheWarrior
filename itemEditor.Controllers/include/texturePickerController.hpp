#pragma once

#include "textureContainer.hpp"
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
    //std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
private:
    const TextureContainer &m_textureContainer;
};