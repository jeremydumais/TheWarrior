#include "textureContainer.hpp"
#include <fmt/format.h>
#include <stdexcept>

using namespace std;

TextureContainer::TextureContainer() 
    : textures(vector<Texture>())
{
}
const std::string &TextureContainer::getLastError() const
{
    return lastError;
}

const vector<Texture>& TextureContainer::getTextures() const
{
    return textures;
}

size_t TextureContainer::getCount() const
{
    return textures.size();
}

boost::optional<const Texture &> TextureContainer::getTextureByName(const std::string &name) const
{
    for(const auto &texture : textures) {
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
        lastError = fmt::format("The texture name {0} already exist in the list", textureInfo.name);
        return false;
    }
    try {
        textures.emplace_back(textureInfo);
    }
    catch(invalid_argument &err) {
        lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::replaceTexture(const std::string &name, const TextureInfo &textureInfo) 
{
    //Find the texture to replace
    auto iter { getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    //Ensure the new name doesn't exist
    if (name != textureInfo.name && getTextureIterator(textureInfo.name) != textures.end()) {
        lastError = fmt::format("The texture {0} already exist in the texture list.", textureInfo.name);
        return false;
    }
    //Try to construct the new texture
    try {
        Texture newTexture(textureInfo);
        swap(*iter, newTexture);
    }
    catch(invalid_argument &err) {
        lastError = err.what();
        return false;
    }
    return true;
}

bool TextureContainer::removeTexture(const std::string &name) 
{
    //Find the texture to delete
    auto iter { getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    textures.erase(iter);
    return true;
}

vector<Texture>::iterator TextureContainer::getTextureIterator(const string &name)
{
    return find_if(textures.begin(), textures.end(), [&name](const Texture &x) {
        return x.getName() == name;
    });
}