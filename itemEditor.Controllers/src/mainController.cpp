#include "mainController.hpp"

MainController::MainController() 
    : m_itemStore(ItemStore()),
      m_textureContainer(TextureContainer())
{
    m_textureContainer.addTexture({"Tex1bhjbkjhbhjb", "Tex1.png", 256, 256, 16, 16});
    m_textureContainer.addTexture({"Tex2bjhbjhb", "Tex2.png", 512, 512, 32, 32});
}

TextureContainer& MainController::getTextureContainerForEdition() 
{
    return m_textureContainer;
}

bool MainController::saveItemStore(const std::string &fileName) 
{
    
}
