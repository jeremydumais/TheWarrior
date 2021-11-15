#pragma once

#include "textureContainer.hpp"
#include <vector>

class ManageTextureController
{
public:
    ManageTextureController(TextureContainer &textureContainer);
    const std::vector<Texture> &getTextures() const;
    std::vector<std::string> getAlreadyUsedNames() const;
private:
    TextureContainer m_textureContainer;
};