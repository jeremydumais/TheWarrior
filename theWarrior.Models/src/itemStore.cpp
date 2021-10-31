#include "itemStore.hpp"

using namespace std;

ItemStore::ItemStore() 
    : textures(vector<Texture>()),
      items(unordered_map<string, Item>())
{
}

size_t ItemStore::getTextureCount() const
{
    return textures.size();
}

size_t ItemStore::getItemCount() const
{
    return items.size();
}

