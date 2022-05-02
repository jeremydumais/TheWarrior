#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>

class AddItemController 
{
public:
    AddItemController(std::shared_ptr<ItemStore> itemStore);
    std::shared_ptr<ItemStore> getItemStore();
    const TextureContainer &getTextureContainer() const;
private:
    std::shared_ptr<ItemStore> m_itemStore;
};