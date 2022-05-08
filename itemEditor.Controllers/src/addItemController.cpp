#include "addItemController.hpp"
#include <fmt/format.h>
#include <memory>
#include <stdexcept>

AddItemController::AddItemController(std::shared_ptr<ItemStore> itemStore)
    : m_itemStore(itemStore),
      m_lastError("")
{
}

const std::string& AddItemController::getLastError() const
{
    return m_lastError;
}

std::shared_ptr<ItemStore> AddItemController::getItemStore()
{
    return m_itemStore;
}

const TextureContainer &AddItemController::getTextureContainer() const
{
    return m_itemStore->getTextureContainer();
}

bool AddItemController::addItem(const ItemCreationInfo &itemInfo)
{
    std::shared_ptr<Item> newItem = nullptr;
    try {
        newItem = std::make_shared<Item>(itemInfo);
    }
    catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }

    return addItemToStore(newItem);
}

bool AddItemController::addItemToStore(std::shared_ptr<Item> newItem)
{
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
