#pragma once

#include <memory>
#include <string>
#include "itemStore.hpp"
#include "textureContainer.hpp"

namespace itemeditor::controllers {

struct ItemDTO {
    std::string id;
    std::string name;
    std::string textureName;
    int textureIndex = -1;
    std::string optionalDescription;
    unsigned int defaultBuyPrice = 0;
    unsigned int defaultSellPrice = 0;
    virtual ~ItemDTO() = default;  //Used to make the struct polymorphic
};

class ManageItemController {
 public:
    explicit ManageItemController(std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    virtual ~ManageItemController() = default;
    const std::string &getLastError() const;
    const thewarrior::models::TextureContainer &getTextureContainer() const;
    virtual std::unique_ptr<ItemDTO> getItem(const std::string &id) const;
    bool addItem(std::unique_ptr<ItemDTO> itemInfo);
    bool updateItem(std::unique_ptr<ItemDTO> itemInfo,
                    const std::string &oldItemId);
    bool deleteItem(const std::string &itemId);

 protected:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::string m_lastError;
    std::shared_ptr<thewarrior::models::ItemStore> getItemStore();
    virtual std::shared_ptr<thewarrior::models::Item> itemDTOToItem(std::unique_ptr<ItemDTO> dto);
};

}  // namespace itemeditor::controllers
