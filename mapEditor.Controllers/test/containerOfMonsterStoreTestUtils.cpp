#include "containerOfMonsterStoreTestUtils.hpp"
#include "monster.hpp"
#include "monsterStore.hpp"
#include "types.hpp"
#include <memory>

using namespace mapeditor::controllers;
using namespace thewarrior::models;

namespace mapeditor::controllers::test {

MonsterCreationInfo ContainerOfMonsterStoreTestUtils::getMonsterSample(const std::string &id,
                                                                       const std::string &name)
{
    return MonsterCreationInfo {
        id,
        name,
        "Tex1",
        1, 15, 1.0F, 1.1F, 2, 4
    };
}

std::shared_ptr<ContainerOfMonsterStore> ContainerOfMonsterStoreTestUtils::getMonsterStores2Samples()
{
    auto stores = std::make_shared<ContainerOfMonsterStore>();
    auto storeB = std::make_shared<MonsterStore>();
    storeB->addMonster(std::make_shared<Monster>(getMonsterSample("DRG001", "Blue Dragon")));
    stores->insert({ "StoreB", storeB });
    auto storeA = std::make_shared<MonsterStore>();
    storeA->addMonster(std::make_shared<Monster>(getMonsterSample("SLM001", "Blue Slime")));
    storeA->addMonster(std::make_shared<Monster>(getMonsterSample("SLM002", "Red Slime")));
    stores->insert({ "StoreA", storeA });
    return stores;
}

} // namespace mapeditor::controllers::test
