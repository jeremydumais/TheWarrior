#include "mainController.hpp"
#include "itemStoreStorage.hpp"
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <set>

MainController::MainController() 
    : m_itemStore(std::make_shared<ItemStore>()),
      m_lastError("")
{
}

TextureContainer &MainController::getTextureContainerForEdition() 
{
    return m_itemStore->getTextureContainerForEdition();
}

std::shared_ptr<ItemStore> MainController::getItemStore()
{
    return m_itemStore;
}

const std::string &MainController::getLastError() const 
{
    return m_lastError;
}

bool MainController::openItemStore(const std::string &fileName) {
    ItemStoreStorage storage;
    try {
        storage.loadItemStore(fileName, m_itemStore);      
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool MainController::saveItemStore(const std::string &fileName) 
{
    ItemStoreStorage storage;  
    try {
        storage.saveItemStore(fileName, m_itemStore);      
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

std::set<std::string> MainController::getItemCategories() const
{
    std::set<std::string> categoriesFound {};
    auto items = m_itemStore->getItems();
    std::transform(items.begin(),
                   items.end(),
                   std::inserter(categoriesFound, categoriesFound.begin()),
                   [] (std::shared_ptr<Item> item) {
                       return itemTypeToString(item->getType());
                   });
    return categoriesFound;
}

std::vector<ItemListDisplay> MainController::getItemsFromCategory(const std::string &categoryName) const
{
    std::vector<ItemListDisplay> retval {};
    auto itemType = itemTypeFromString(categoryName);
    if (itemType.has_value()) {
        auto items = m_itemStore->getItems();
        for(auto item : items) {
            if (item->getType() == itemType.value()) {
                retval.push_back({ item->getId(), item->getName()});
            }
        }
    }
    return retval;
}

std::optional<ItemType> MainController::getItemTypeFromItemId(const std::string &id) const
{
    auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
        return item->getType();
    }
    return std::nullopt;
}
