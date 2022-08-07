#include "manageItemController.hpp"
#include <fmt/format.h>
#include <memory>
#include <stdexcept>

using namespace thewarrior::models;

namespace itemeditor::controllers {

ManageItemController::ManageItemController(std::shared_ptr<ItemStore> itemStore)
    : m_itemStore(itemStore),
      m_lastError("")
{
}

const std::string& ManageItemController::getLastError() const
{
    return m_lastError;
}

std::shared_ptr<ItemStore> ManageItemController::getItemStore()
{
    return m_itemStore;
}

const TextureContainer &ManageItemController::getTextureContainer() const
{
    return m_itemStore->getTextureContainer();
}

std::unique_ptr<ItemDTO> ManageItemController::getItem(const std::string &id) const
{
    auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
        auto retval = std::make_unique<ItemDTO>();
        retval->id = item->getId();
        retval->name = item->getName();
        retval->textureName = item->getTextureName();
        retval->textureIndex = item->getTextureIndex();
        retval->optionalDescription = item->getOptionalDescription();
        return retval;
    }
    return nullptr;
}

bool ManageItemController::addItem(std::unique_ptr<ItemDTO> itemInfo)
{
    std::shared_ptr<Item> newItem = itemDTOToItem(std::move(itemInfo));
    if (newItem == nullptr) {
        return false;
    }
    if (m_itemStore->isItemExists(newItem->getId())) {
        m_lastError = fmt::format("Item {0} already exist in the store.", newItem->getId());
        return false;
    }
    if (!m_itemStore->addItem(newItem)) {
        m_lastError = m_itemStore->getLastError();
        return false;
    }
    return true;
}

bool ManageItemController::updateItem(std::unique_ptr<ItemDTO> itemInfo,
                                      const std::string &oldItemId)
{
    if (itemInfo == nullptr) {
        m_lastError = "No itemInfo structure has been provided.";
        return false;
    }
    std::shared_ptr<Item> updateItem = itemDTOToItem(std::move(itemInfo));
    if (updateItem == nullptr) {
        return false;
    }
    if (updateItem->getId() != oldItemId && m_itemStore->isItemExists(updateItem->getId())) {
        m_lastError = fmt::format("Item {0} already exist in the store.", updateItem->getId());
        return false;
    }
    if (!m_itemStore->replaceItem(oldItemId, updateItem)) {
        m_lastError = m_itemStore->getLastError();
        return false;
    }
    return true;
}

bool ManageItemController::deleteItem(const std::string &itemId)
{
    if (!m_itemStore->removeItem(itemId)) {
        m_lastError = m_itemStore->getLastError();
        return false;
    }
    return true;
}

std::shared_ptr<Item> ManageItemController::itemDTOToItem(std::unique_ptr<ItemDTO> dto)
{
    ItemCreationInfo creationInfo = {
        dto->id,
        dto->name,
        dto->textureName,
        dto->textureIndex,
        dto->optionalDescription
    };
    std::shared_ptr<Item> updateItem = nullptr;
    try {
        updateItem = std::make_shared<Item>(creationInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
    }
    return updateItem;
}

} // namespace itemeditor::controllers
