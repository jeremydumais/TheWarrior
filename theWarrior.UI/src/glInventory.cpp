#include "glInventory.hpp"

GLInventory::GLInventory()
    : m_inventory(nullptr)
{
}

void GLInventory::setInventory(std::shared_ptr<Inventory> inventory)
{
    m_inventory = inventory;
}
