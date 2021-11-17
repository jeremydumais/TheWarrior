#include "manageTexturesController.hpp"
#include <algorithm>

using namespace std;

ManageTextureController::ManageTextureController(TextureContainer &textureContainer) 
    : m_textureContainer(textureContainer)
{
}

const vector<Texture>& ManageTextureController::getTextures() const
{
    return m_textureContainer.getTextures();
}

optional<reference_wrapper<const Texture>> ManageTextureController::getTextureByName(const std::string &name) const
{    
    return m_textureContainer.getTextureByName(name);
} 

vector<string> ManageTextureController::getAlreadyUsedNames() const
{
    vector<string> textureNames;
    const auto &textures { m_textureContainer.getTextures() };
    transform(textures.begin(), 
              textures.end(), 
              back_inserter(textureNames),
              [](const Texture & x) { return x.getName(); });
    return textureNames;
}

bool ManageTextureController::addTexture(const TextureInfo &info) 
{
    return m_textureContainer.addTexture(info);
}

bool ManageTextureController::replaceTexture(const std::string &name, const TextureInfo &updatedTexture) 
{
    return m_textureContainer.replaceTexture(name, updatedTexture);

}