#include "itemStore.hpp"

using namespace std;

ItemStore::ItemStore() 
    : lastError(""),
      textureContainer(TextureContainer()),
      items(unordered_map<string, Item>())
{
}

const std::string &ItemStore::getLastError() const
{
    return lastError;
}

size_t ItemStore::getTextureCount() const
{
    return textureContainer.getCount();
}

size_t ItemStore::getItemCount() const
{
    return items.size();
}

const vector<Texture> &ItemStore::getTextures() const
{
    return textureContainer.getTextures();
}
  
boost::optional<const Texture &> ItemStore::getTextureByName(const string &name) const
{
    return textureContainer.getTextureByName(name);
}

bool ItemStore::addTexture(const TextureInfo &textureInfo)
{
    bool retVal = textureContainer.addTexture(textureInfo);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

bool ItemStore::replaceTexture(const std::string &name, const TextureInfo &textureInfo)
{
    bool retVal = textureContainer.replaceTexture(name, textureInfo);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

bool ItemStore::removeTexture(const std::string &name)
{
    bool retVal = textureContainer.removeTexture(name);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

boost::optional<const Item &> ItemStore::findItem(const std::string &id) const
{
    const auto iter = items.find(id);
    if (iter != items.end()) {
        return iter->second;
    }
    else {
        return {};
    }
}

bool ItemStore::addItem(const Item &item) 
{
    bool wasInserted = items.insert({ item.getId(), item }).second;
    return wasInserted;
}

bool ItemStore::replaceItem(const string oldId, const Item &item) 
{
    //Check if the old item name specified exist
    auto iter = items.find(oldId);
    if (iter == items.end()) {
        return false;
    }
    return true;
}

