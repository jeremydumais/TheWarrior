#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
private:
    ItemStore itemStore;
    TextureContainer textureContainer;
};