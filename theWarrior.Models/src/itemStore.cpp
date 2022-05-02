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

size_t ItemStore::getItemCount() const
{
    return m_items.size();
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

const TextureContainer &ItemStore::getTextureContainer() const
{
    return m_textureContainer;
}

TextureContainer &ItemStore::getTextureContainerForEdition()
{
    return m_textureContainer;
}
