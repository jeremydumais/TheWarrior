#include "mainController.hpp"

MainController::MainController() 
    : itemStore(ItemStore()),
      textureContainer(TextureContainer())
{
    textureContainer.addTexture({"Tex1bhjbkjhbhjb", "Tex1.png", 256, 256, 16, 16});
    textureContainer.addTexture({"Tex2bjhbjhb", "Tex2.png", 512, 512, 32, 32});
}

TextureContainer& MainController::getTextureContainerForEdition() 
{
    return textureContainer;
}
