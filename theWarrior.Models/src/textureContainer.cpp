#include <fmt/format.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "textureContainer.hpp"

namespace thewarrior::models {

const std::string &TextureContainer::getLastError() const {
    return m_lastError;
}

const std::vector<Texture>& TextureContainer::getTextures() const {
    return m_textures;
}

size_t TextureContainer::getCount() const {
    return m_textures.size();
}

std::optional<std::reference_wrapper<const Texture>> TextureContainer::getTextureByName(const std::string &name) const {
    for (const auto &texture : m_textures) {
        if (texture.getName() == name) {
            return std::optional<std::reference_wrapper<const Texture>>{texture};
        }
    }
    return std::nullopt;
}

bool TextureContainer::addTexture(const TextureInfo &textureInfo) {
    // Check that name doesn't already exist in the list
    if (getTextureByName(textureInfo.name).has_value()) {
        m_lastError = fmt::format("The texture name {0} already exist in the list", textureInfo.name);
        return false;
    }
    try {
        m_textures.emplace_back(textureInfo);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::replaceTexture(const std::string &name, const TextureInfo &textureInfo) {
    // Find the texture to replace
    auto iter { getTextureIterator(name) };
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    // Ensure the new name doesn't exist
    if (name != textureInfo.name && getTextureIterator(textureInfo.name) != m_textures.end()) {
        m_lastError = fmt::format("The texture {0} already exist in the texture list.", textureInfo.name);
        return false;
    }
    // Try to construct the new texture
    try {
        Texture newTexture(textureInfo);
        std::swap(*iter, newTexture);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::removeTexture(const std::string &name) {
    // Find the texture to delete
    auto iter { getTextureIterator(name) };
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    m_textures.erase(iter);
    return true;
}

std::vector<Texture>::iterator TextureContainer::getTextureIterator(const std::string &name) {
    return std::ranges::find_if(m_textures, [&name](const Texture &x) {
        return x.getName() == name;
    });
}

}  // namespace thewarrior::models
