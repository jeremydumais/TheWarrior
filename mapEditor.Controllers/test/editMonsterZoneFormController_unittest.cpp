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
        controller->addMonsterEncounter(MonsterEncounterDTO {
            .monsterId = "DRA001",
            .monsterName = "Blue Dragon",
            .encounterRatio = "Normal",
            .monsterIcon = std::nullopt });
        controller->addMonsterEncounter(MonsterEncounterDTO {
            .monsterId = "SLM001",
            .monsterName = "Blue Slime",
            .encounterRatio = "Less than normal",
            .monsterIcon = std::nullopt });
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

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, updateMonsterEncounter_WithRare1_ReturnFalse)
{
    const MonsterEncounterDTO expected {
        .monsterId = "DRA002",
        .monsterName = "",
        .encounterRatio = "Rare1",
        .monsterIcon = std::nullopt
    };
    ASSERT_FALSE(controller->updateMonsterEncounter("DRA001", expected));
    ASSERT_EQ("Encounter ratio Rare1 cannot be determined.", controller->getLastError());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, updateMonsterEncounter_WithDDD001_ReturnFalse)
{
    const MonsterEncounterDTO expected {
        .monsterId = "DRA002",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_FALSE(controller->updateMonsterEncounter("DDD001", expected));
    ASSERT_EQ("Unable to find the old monster id DDD001.", controller->getLastError());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, updateMonsterEncounter_WithDRA001Andslm001_ReturnFalse)
{
    const MonsterEncounterDTO expected {
        .monsterId = "slm001",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_FALSE(controller->updateMonsterEncounter("DRA001", expected));
    ASSERT_EQ("The new monster id slm001 is already part of the zone.", controller->getLastError());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, updateMonsterEncounter_WithDRA001AndSLM003_ReturnTrue)
{
    const MonsterEncounterDTO expected {
        .monsterId = "SLM003",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_TRUE(controller->updateMonsterEncounter("DRA001", expected));
    const auto &monsterEncounters = controller->getMonsterEncounters();
    ASSERT_EQ(expected.monsterId, monsterEncounters[0].monsterId);
    ASSERT_EQ(expected.encounterRatio, monsterEncounters[0].encounterRatio);
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, updateMonsterEncounter_WithDRA001AndDRA001_ReturnTrue)
{
    const MonsterEncounterDTO expected {
        .monsterId = "DRA001",
        .monsterName = "",
        .encounterRatio = "Rare",
        .monsterIcon = std::nullopt
    };
    ASSERT_TRUE(controller->updateMonsterEncounter("DRA001", expected));
    const auto &monsterEncounters = controller->getMonsterEncounters();
    ASSERT_EQ(expected.monsterId, monsterEncounters[0].monsterId);
    ASSERT_EQ(expected.encounterRatio, monsterEncounters[0].encounterRatio);
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, removeMonsterEncounter_WithDDD001_ReturnFalse)
{
    ASSERT_FALSE(controller->removeMonsterEncounter("DDD001"));
    ASSERT_EQ("Unable to find the monster id DDD001.", controller->getLastError());
}

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, removeMonsterEncounter_WithDRA001_ReturnTrue)
{
    ASSERT_TRUE(controller->removeMonsterEncounter("DRA001"));
    const auto &monsterEncounters = controller->getMonsterEncounters();
    ASSERT_EQ(1, monsterEncounters.size());
    ASSERT_EQ("SLM001", monsterEncounters[0].monsterId);
}

} // namespace mapeditor::controllers::editmonsterzoneformcontroller
