#include "mainController.hpp"
#include "itemStoreStorage.hpp"
#include <stdexcept>

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
