#include <gtest/gtest.h>
#include "containerOfMonsterStoreTestUtils.hpp"
#include "monster.hpp"
#include "monsterPickerFormController.hpp"
#include "monsterStore.hpp"
#include "types.hpp"
#include <memory>

using namespace mapeditor::controllers;
using namespace mapeditor::controllers::test;
using namespace thewarrior::models;

namespace mapeditor::controllers::monsterpickerformcontroller::unittest {

class MonsterPickerFormControllerWith2StoresSample : public ::testing::Test
{
public:
    MonsterPickerFormControllerWith2StoresSample()
    {
        controller = std::make_shared<MonsterPickerFormController>(ContainerOfMonsterStoreTestUtils::getMonsterStores2Samples(),
                                                                   "FakeResourcesPath");
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

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithEmptyStoreName_ReturnEmpty)
{
    ASSERT_EQ(0, controller->getMonsterIds("", "").size());
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreANoFilter_Return2Monsters)
{
    const auto monsters = controller->getMonsterIds("StoreA", "");
    ASSERT_EQ(2, monsters.size());
    ASSERT_EQ("SLM001", monsters[0]);
    ASSERT_EQ("SLM002", monsters[1]);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreAAndBlue_Return1Monster)
{
    const auto monsters = controller->getMonsterIds("StoreA", "Blue");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM001", monsters[0]);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreAAnd001_Return1Monster)
{
    const auto monsters = controller->getMonsterIds("StoreA", "001");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM001", monsters[0]);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreAAnd2_Return1Monster)
{
    const auto monsters = controller->getMonsterIds("StoreA", "2");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("SLM002", monsters[0]);
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreBAnd2_Return0Monster)
{
    const auto monsters = controller->getMonsterIds("StoreB", "2");
    ASSERT_EQ(0, monsters.size());
}

TEST_F(MonsterPickerFormControllerWith2StoresSample, getMonsterIds_WithStoreBAndDra_Return0Monster)
{
    const auto monsters = controller->getMonsterIds("StoreB", "Dra");
    ASSERT_EQ(1, monsters.size());
    ASSERT_EQ("DRG001", monsters[0]);
}

} // namespace mapeditor::controllers::monsterpickerformcontroller::unittest
