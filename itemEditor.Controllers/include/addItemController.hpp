#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <string>

class AddItemController 
{
public:
    AddItemController(std::shared_ptr<ItemStore> itemStore);
    const std::string &getLastError() const;
    std::shared_ptr<ItemStore> getItemStore();
    const TextureContainer &getTextureContainer() const;
    bool addItem(const ItemCreationInfo &itemInfo);
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
};