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
