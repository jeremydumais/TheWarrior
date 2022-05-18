#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <string>

struct ItemDTO 
{
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex;
    virtual ~ItemDTO() = default; //Used to make the struct polymorphic
};

class ManageItemController
{
public:
    ManageItemController(std::shared_ptr<ItemStore> itemStore);
    virtual ~ManageItemController() = default;
    const std::string &getLastError() const;
    const TextureContainer &getTextureContainer() const;
    virtual std::unique_ptr<ItemDTO> getItem(const std::string &id) const;
    bool addItem(std::unique_ptr<ItemDTO> itemInfo);
    bool updateItem(std::unique_ptr<ItemDTO> itemInfo,
                            const std::string &oldItemId);
protected:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
    std::shared_ptr<ItemStore> getItemStore();
    virtual std::shared_ptr<Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto);
};