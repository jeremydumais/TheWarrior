#include "itemStore.hpp"
#include <algorithm>

using namespace std;

namespace thewarrior::models {

ItemStore::ItemStore()
    : m_lastError(""),
      m_textureContainer(TextureContainer()),
      m_items(unordered_map<string, std::shared_ptr<Item>>())
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

std::vector<std::shared_ptr<Item>> ItemStore::getItems() const
{
    std::vector<std::shared_ptr<Item>> retval {};
    std::transform(m_items.begin(),
                   m_items.end(),
                   std::back_inserter(retval),
                   [] (std::pair<const std::string &, std::shared_ptr<Item>>item) {
                       return std::reference_wrapper(item.second);
                   });
    std::sort(retval.begin(), retval.end(), [](std::shared_ptr<Item> a,
                                               std::shared_ptr<Item> b) {
        return a->getId() < b->getId();
    });
    return retval;
}

const std::shared_ptr<const Item> ItemStore::findItem(const std::string &id) const
{
    const auto iter = m_items.find(id);
    if (iter != m_items.end()) {
        return iter->second;
    }
    else {
        return nullptr;
    }
}

bool ItemStore::isItemExists(const std::string &id) const
{
    return this->findItem(id) != nullptr;
}


bool ItemStore::addItem(std::shared_ptr<Item> item)
{
    bool wasInserted = m_items.insert({ item->getId(), item }).second;
    return wasInserted;
}

bool ItemStore::replaceItem(const string oldId, std::shared_ptr<Item> item)
{
    //Check if the old item name specified exist
    const auto iter = m_items.find(oldId);
    if (iter == m_items.end()) {
        return false;
    }
    if (m_items.erase(oldId) == 0) {
        return false;
    }
    bool wasInserted = m_items.insert({ item->getId(), item }).second;
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

} // namespace thewarrior::models
