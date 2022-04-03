#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <string>

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
    bool saveItemStore(const std::string &fileName);
private:
    ItemStore m_itemStore;
    TextureContainer m_textureContainer;
};