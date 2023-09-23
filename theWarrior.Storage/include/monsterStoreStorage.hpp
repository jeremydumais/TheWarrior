#pragma once

#include "monsterStore.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

namespace thewarrior::storage {

class MonsterStoreStorage
{
public:
    void loadMonsterStore(const std::string &fileName, std::shared_ptr<thewarrior::models::MonsterStore> monsterStore);
    void saveMonsterStore(const std::string &fileName, std::shared_ptr<thewarrior::models::MonsterStore> monsterStore);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::MonsterStore>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::MonsterStore>> m_bfs;
};

} // namespace thewarrior::storage
