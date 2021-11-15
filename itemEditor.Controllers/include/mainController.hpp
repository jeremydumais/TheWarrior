#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
private:
    ItemStore m_itemStore;
    TextureContainer m_textureContainer;
};