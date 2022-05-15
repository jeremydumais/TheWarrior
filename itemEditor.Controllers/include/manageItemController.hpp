#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <string>

class ManageItemController
{
public:
    ManageItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageItemController() = default;
    const std::string &getLastError() const;
    std::shared_ptr<ItemStore> getItemStore();
    const TextureContainer &getTextureContainer() const;
    virtual bool addItem(const ItemCreationInfo &itemInfo);
protected:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
    bool addItemToStore(std::shared_ptr<Item> item);
};