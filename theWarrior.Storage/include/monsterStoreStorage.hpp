#pragma once

#include "monsterStore.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

class MonsterStoreStorage 
{
public:
    void loadMonsterStore(const std::string &fileName, std::shared_ptr<MonsterStore> monsterStore);
    void saveMonsterStore(const std::string &fileName, std::shared_ptr<MonsterStore> monsterStore);
    void setFileStream(std::unique_ptr<IBinaryFileStream<MonsterStore>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<MonsterStore>> m_bfs;
};