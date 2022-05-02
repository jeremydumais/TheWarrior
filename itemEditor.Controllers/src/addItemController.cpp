#include "addItemController.hpp"

AddItemController::AddItemController(std::shared_ptr<ItemStore> itemStore)
    : m_itemStore(itemStore)
{
}

std::shared_ptr<ItemStore> AddItemController::getItemStore()
{
    return m_itemStore;
}

const TextureContainer &AddItemController::getTextureContainer() const
{
    return m_itemStore->getTextureContainer();
}
