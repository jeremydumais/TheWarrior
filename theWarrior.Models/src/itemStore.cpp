#include "itemStore.hpp"
#include <algorithm>

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

std::vector<std::reference_wrapper<const Item>> ItemStore::getItems() const
{
    std::vector<std::reference_wrapper<const Item>> retval {};
    std::transform(m_items.begin(), 
                   m_items.end(), 
                   std::back_inserter(retval),
                   [] (std::pair<const std::string &, const Item &>item) {
                       return std::reference_wrapper(item.second);
                   });
    std::sort(retval.begin(), retval.end(), [](std::reference_wrapper<const Item> a,
                                               std::reference_wrapper<const Item> b) {
        return a.get().getId() < b.get().getId();
    });
    return retval;
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

bool ItemStore::isItemExists(const std::string &id) const
{
    return this->findItem(id).has_value();
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
