#include "textureContainer.hpp"
#include <fmt/format.h>
#include <stdexcept>

using namespace std;

TextureContainer::TextureContainer() 
    : m_textures(vector<Texture>())
{
}
const std::string &TextureContainer::getLastError() const
{
    return m_lastError;
}

const vector<Texture>& TextureContainer::getTextures() const
{
    return m_textures;
}

size_t TextureContainer::getCount() const
{
    return m_textures.size();
}

boost::optional<const Texture &> TextureContainer::getTextureByName(const std::string &name) const
{
    for(const auto &texture : m_textures) {
        if (texture.getName() == name) {
            return texture;
        }
    }
    return {};
}

bool TextureContainer::addTexture(const TextureInfo &textureInfo) 
{
    //Check that name doesn't already exist in the list
    if (getTextureByName(textureInfo.name).has_value()) {
        m_lastError = fmt::format("The texture name {0} already exist in the list", textureInfo.name);
        return false;
    }
    try {
        m_textures.emplace_back(textureInfo);
    }
    catch(invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::replaceTexture(const std::string &name, const TextureInfo &textureInfo) 
{
    //Find the texture to replace
    auto iter { getTextureIterator(name) };
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    //Ensure the new name doesn't exist
    if (name != textureInfo.name && getTextureIterator(textureInfo.name) != m_textures.end()) {
        m_lastError = fmt::format("The texture {0} already exist in the texture list.", textureInfo.name);
        return false;
    }
    //Try to construct the new texture
    try {
        Texture newTexture(textureInfo);
        swap(*iter, newTexture);
    }
    catch(invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::removeTexture(const std::string &name) 
{
    //Find the texture to delete
    auto iter { getTextureIterator(name) };
    if (iter == m_textures.end()) {
        m_lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    m_textures.erase(iter);
    return true;
}

vector<Texture>::iterator TextureContainer::getTextureIterator(const string &name)
{
    return find_if(m_textures.begin(), m_textures.end(), [&name](const Texture &x) {
        return x.getName() == name;
    });
}