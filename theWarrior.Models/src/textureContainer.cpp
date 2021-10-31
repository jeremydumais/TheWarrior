#include "textureContainer.hpp"

using namespace std;

TextureContainer::TextureContainer() 
    : textures(vector<Texture>())
{
}

size_t TextureContainer::getCount() const
{
    return textures.size();
}
