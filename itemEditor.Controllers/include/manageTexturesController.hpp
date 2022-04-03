#pragma once

#include "textureContainer.hpp"
#include <optional>
#include <vector>

class ManageTextureController
{
public:
    ManageTextureController(TextureContainer &textureContainer);
    const std::vector<Texture> &getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
    std::vector<std::string> getAlreadyUsedNames() const;
    bool addTexture(const TextureInfo &info);
    bool replaceTexture(const std::string &name, const TextureInfo &updatedTexture);
    bool removeTexture(const std::string &name);
private:
    TextureContainer &m_textureContainer;
};