#include "manageTexturesController.hpp"

using namespace std;

ManageTextureController::ManageTextureController(TextureContainer &textureContainer) 
    : textureContainer(textureContainer)
{
}

const vector<Texture>& ManageTextureController::getTextures() const
{
    return textureContainer.getTextures();
}

vector<string> ManageTextureController::getAlreadyUsedNames() const
{
    vector<string> textureNames;
    const auto &textures { textureContainer.getTextures() };
    transform(textures.begin(), 
              textures.end(), 
              back_inserter(textureNames),
              [](const Texture & x) { return x.getName(); });
    return textureNames;
}
