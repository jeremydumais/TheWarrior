#include <gtest/gtest.h>
#include "monster.hpp"
#include "monsterPickerFormController.hpp"
#include "monsterStore.hpp"
#include "types.hpp"
#include <memory>

using namespace mapeditor::controllers;
using namespace thewarrior::models;

namespace mapeditor::controllers::monsterpickerformcontroller::unittest {

MonsterCreationInfo getMonsterSample(const std::string &id, const std::string &name)
{
    return MonsterCreationInfo {
        id,
        name,
        "Tex1",
        1, 15, 1.0F, 1.1F, 2, 4
    };
}

class MonsterPickerFormControllerWith2StoresSample : public ::testing::Test
{
public:
    MonsterPickerFormControllerWith2StoresSample()
    {
        auto stores = std::make_shared<ContainerOfMonsterStore>();
        auto storeB = std::make_shared<MonsterStore>();
        storeB->addMonster(std::make_shared<Monster>(getMonsterSample("DRG001", "Blue Dragon")));
        stores->insert({ "StoreB", storeB });
        auto storeA = std::make_shared<MonsterStore>();
        storeA->addMonster(std::make_shared<Monster>(getMonsterSample("SLM001", "Blue Slime")));
        storeA->addMonster(std::make_shared<Monster>(getMonsterSample("SLM002", "Red Slime")));
        stores->insert({ "StoreA", storeA });
        controller = std::make_shared<MonsterPickerFormController>(stores);
    }
    std::shared_ptr<MonsterPickerFormController> controller = nullptr;
};

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterStoreNames_Return2Names)
{
    const auto names = controller->getMonsterStoreNames();
    ASSERT_EQ(2, names.size());
    ASSERT_EQ("StoreA", names[0]);
    ASSERT_EQ("StoreB", names[1]);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithEmptyStoreName_ReturnEmpty)
{
    ASSERT_EQ(0, controller->getMonsters("", "").size());
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreANoFilter_Return2Monsters)
{
    const auto monsters = controller->getMonsters("StoreA", "");
    ASSERT_EQ(2, monsters.size());
    ASSERT_EQ("SLM001", monsters[0].id);
    ASSERT_EQ("SLM002", monsters[1].id);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreAAndBlue_Return1Monster)
{
    const auto monsters = controller->getMonsters("StoreA", "Blue");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM001", monsters[0].id);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreAAnd001_Return1Monster)
{
    const auto monsters = controller->getMonsters("StoreA", "001");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM001", monsters[0].id);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreAAnd2_Return1Monster)
{
    const auto monsters = controller->getMonsters("StoreA", "2");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM002", monsters[0].id);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreBAnd2_Return0Monster)
{
    const auto monsters = controller->getMonsters("StoreB", "2");
    ASSERT_EQ(0, monsters.size());
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsters_WithStoreBAndDra_Return0Monster)
{
    const auto monsters = controller->getMonsters("StoreB", "Dra");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("DRG001", monsters[0].id);
}

} // namespace mapeditor::controllers::monsterpickerformcontroller::unittest
