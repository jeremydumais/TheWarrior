#pragma once

#include "itemStore.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

namespace thewarrior::storage {

class ItemStoreStorage
{
public:
    void loadItemStore(const std::string &fileName, std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    void saveItemStore(const std::string &fileName, std::shared_ptr<thewarrior::models::ItemStore> itemStore);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::ItemStore>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::ItemStore>> m_bfs;
};

} // namespace thewarrior::storage
