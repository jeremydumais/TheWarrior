#pragma once

#include "monster.hpp"
#include "types.hpp"
#include <memory>
#include <string>

namespace mapeditor::controllers::test {

class ContainerOfMonsterStoreTestUtils
{
public:
    static thewarrior::models::MonsterCreationInfo getMonsterSample(const std::string &id,
                                                                    const std::string &name);
    static std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> getMonsterStores2Samples();
};

} // namespace mapeditor::controllers::test
