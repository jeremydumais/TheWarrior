#include <gtest/gtest.h>
#include "editMonsterZoneFormController.hpp"
#include "containerOfMonsterStoreTestUtils.hpp"
#include "monsterEncounterDTO.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <optional>
#include <stdexcept>

using namespace mapeditor::controllers;
using namespace mapeditor::controllers::test;
using namespace thewarrior::models;

namespace mapeditor::controllers::editmonsterzoneformcontroller::unittest {

class EditMonsterZoneFormControllerWith2StoresSample : public ::testing::Test
{
public:
    EditMonsterZoneFormControllerWith2StoresSample()
    {

        controller = std::make_shared<EditMonsterZoneFormController>(ContainerOfMonsterStoreTestUtils::getMonsterStores2Samples(),
                                                                     "fakeResourcesPath");
    }
    std::shared_ptr<EditMonsterZoneFormController> controller = nullptr;
    std::vector<std::string> alreadyUsedMonsterIds = { "SLM001", "DRA001" };
};

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, getResourcesPath_ReturnFakeResourcesPath)
{
    ASSERT_EQ("fakeResourcesPath", controller->getResourcesPath());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, addMonsterEncounter_WithTES001_ReturnTrue)
{
    const MonsterEncounterDTO expected {
        .monsterId = "TES001",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_TRUE(controller->addMonsterEncounter(expected));
    const auto items = controller->getMonsterEncounters();
    ASSERT_EQ(3, items.size());
    ASSERT_EQ(expected.monsterId, items[2].monsterId);
    ASSERT_EQ(expected.encounterRatio, items[2].encounterRatio);
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, addMonsterEncounter_WithRare1_ReturnFalse)
{
    const MonsterEncounterDTO expected {
        .monsterId = "TES001",
        .monsterName = "",
        .encounterRatio = "Rare1",
        .monsterIcon = std::nullopt
    };
    ASSERT_FALSE(controller->addMonsterEncounter(expected));
    ASSERT_EQ("Encounter ratio Rare1 cannot be determined.", controller->getLastError());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, addMonsterEncounter_WithDRA001_ReturnFalse)
{
    const MonsterEncounterDTO expected {
        .monsterId = "DRA001",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_FALSE(controller->addMonsterEncounter(expected));
    ASSERT_EQ("Monster DRA001 is already part of the zone.", controller->getLastError());
}

//TODO Test updateMonsterEncounter

} // namespace mapeditor::controllers::editmonsterzoneformcontroller
