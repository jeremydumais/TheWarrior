#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <string>

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
    const std::string &getLastError() const;
    bool saveItemStore(const std::string &fileName);
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
    TextureContainer m_textureContainer;
};