#pragma once

#include "itemStore.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

class ItemStoreStorage 
{
public:
    void loadItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore);
    void saveItemStore(const std::string &fileName, std::shared_ptr<ItemStore> itemStore);
    void setFileStream(std::unique_ptr<IBinaryFileStream<ItemStore>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<ItemStore>> m_bfs;
};