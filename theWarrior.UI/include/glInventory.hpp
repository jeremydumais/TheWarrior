#pragma once

#include "inventory.hpp"
#include <memory>

class GLInventory
{
public:
    GLInventory();
    void setInventory(std::shared_ptr<Inventory> inventory);
private:
    std::shared_ptr<Inventory> m_inventory;
};