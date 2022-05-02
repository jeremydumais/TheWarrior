#pragma once

#include "textureContainer.hpp"
#include <optional>
#include <vector>

class TexturePickerController 
{
public:
    TexturePickerController(const TextureContainer &textureContainer);
    const std::vector<Texture> &getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
private:
    const TextureContainer &m_textureContainer;
};