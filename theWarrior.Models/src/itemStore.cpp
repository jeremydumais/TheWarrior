#include "itemStore.hpp"

using namespace std;

ItemStore::ItemStore() 
    : m_lastError(""),
      m_textureContainer(TextureContainer()),
      m_items(unordered_map<string, Item>())
{
}

const std::string &ItemStore::getLastError() const
{
    return m_lastError;
}

size_t ItemStore::getTextureCount() const
{
    return m_textureContainer.getCount();
}

size_t ItemStore::getItemCount() const
{
    return m_items.size();
}

const vector<Texture> &ItemStore::getTextures() const
{
    return m_textureContainer.getTextures();
}
  
boost::optional<const Texture &> ItemStore::getTextureByName(const string &name) const
{
    return m_textureContainer.getTextureByName(name);
}

bool ItemStore::addTexture(const TextureInfo &textureInfo)
{
    bool retVal = m_textureContainer.addTexture(textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool ItemStore::replaceTexture(const std::string &name, const TextureInfo &textureInfo)
{
    bool retVal = m_textureContainer.replaceTexture(name, textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool ItemStore::removeTexture(const std::string &name)
{
    bool retVal = m_textureContainer.removeTexture(name);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

boost::optional<const Item &> ItemStore::findItem(const std::string &id) const
{
    const auto iter = m_items.find(id);
    if (iter != m_items.end()) {
        return iter->second;
    }
    else {
        return {};
    }
}

bool ItemStore::addItem(const Item &item) 
{
    bool wasInserted = m_items.insert({ item.getId(), item }).second;
    return wasInserted;
}

bool ItemStore::replaceItem(const string oldId, const Item &item) 
{
    //Check if the old item name specified exist
    const auto iter = m_items.find(oldId);
    if (iter == m_items.end()) {
        return false;
    }
    if (m_items.erase(oldId) == 0) {
        return false;
    }
    bool wasInserted = m_items.insert({ item.getId(), item }).second;
    return wasInserted;
}

bool ItemStore::removeItem(const std::string &id) 
{
    if (m_items.erase(id) == 0) {
        return false;
    }
    return true;
}

