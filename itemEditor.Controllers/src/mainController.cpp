#include "mainController.hpp"
#include "itemStoreStorage.hpp"
#include <algorithm>
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
                   [] (std::reference_wrapper<const Item> item) {
                       return item.get().getType();
                   });
    return categoriesFound;
}

std::vector<ItemListDisplay> MainController::getItemsFromCategory(const std::string &categoryName) const
{
    std::vector<ItemListDisplay> retval {};
    auto items = m_itemStore->getItems();
    for(auto item : items) {
        if (item.get().getType() == categoryName) {
            retval.push_back({ item.get().getId(), item.get().getName()});
        }
    }
    return retval;
}
