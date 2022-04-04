#include "mainController.hpp"
#include "itemStoreStorage.hpp"
#include <stdexcept>

MainController::MainController() 
    : m_itemStore(std::make_shared<ItemStore>()),
      m_lastError(""),
      m_textureContainer(TextureContainer())
{
    m_textureContainer.addTexture({"Tex1bhjbkjhbhjb", "Tex1.png", 256, 256, 16, 16});
    m_textureContainer.addTexture({"Tex2bjhbjhb", "Tex2.png", 512, 512, 32, 32});
}

TextureContainer &MainController::getTextureContainerForEdition() 
{
    return m_textureContainer;
}

const std::string &MainController::getLastError() const 
{
    return m_lastError;
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
