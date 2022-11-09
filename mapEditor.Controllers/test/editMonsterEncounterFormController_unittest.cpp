#include <gtest/gtest.h>
#include "editMonsterEncounterFormController.hpp"
#include "containerOfMonsterStoreTestUtils.hpp"
#include "monster.hpp"
#include "monsterStore.hpp"
#include "types.hpp"
#include <memory>
#include <vector>

using namespace mapeditor::controllers;
using namespace mapeditor::controllers::test;
using namespace thewarrior::models;

namespace mapeditor::controllers::editmonsterencounterformcontroller::unittest {


class EditMonsterEncounterFormControllerWith2StoresSample : public ::testing::Test
{
public:
    EditMonsterEncounterFormControllerWith2StoresSample()
    {

        controller = std::make_shared<EditMonsterEncounterFormController>(ContainerOfMonsterStoreTestUtils::getMonsterStores2Samples(),
                                                                          "fakeResourcesPath",
                                                                          alreadyUsedMonsterIds);
    }
    std::shared_ptr<EditMonsterEncounterFormController> controller = nullptr;
    std::vector<std::string> alreadyUsedMonsterIds = { "SLM001", "DRA001" };
};

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, GetMonsterNameById_WithEmptyId_ReturnEmptyString)
{
    ASSERT_EQ("", controller->getMonsterNameById(""));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, GetMonsterNameById_WithWhiteSpacesId_ReturnEmptyString)
{
    ASSERT_EQ("", controller->getMonsterNameById("  "));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, GetMonsterNameById_Withsml001Id_ReturnBlueSlime)
{
    ASSERT_EQ("Blue Slime", controller->getMonsterNameById("slm001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, GetMonsterNameById_WithSLM001Id_ReturnBlueSlime)
{
    ASSERT_EQ("Blue Slime", controller->getMonsterNameById("SLM001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, GetMonsterNameById_WithDRG001Id_ReturnBlueDragon)
{
    ASSERT_EQ("Blue Dragon", controller->getMonsterNameById("DRG001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterIdValue_With3Spaces_ReturnFalse)
{
    ASSERT_FALSE(controller->isMonsterIdValid("   "));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterIdValue_WithTES001_ReturnTrue)
{
    ASSERT_TRUE(controller->isMonsterIdValid("TES001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterAlreadyUsed_WithTES001_ReturnFalse)
{
    ASSERT_FALSE(controller->isMonsterAlreadyUsed("TES001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterAlreadyUsed_WithDRG001_ReturnTrue)
{
    ASSERT_TRUE(controller->isMonsterAlreadyUsed("DRA001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterAlreadyUsed_WithSLM001_ReturnTrue)
{
    ASSERT_TRUE(controller->isMonsterAlreadyUsed("SLM001"));
}

TEST_F(EditMonsterEncounterFormControllerWith2StoresSample, IsMonsterAlreadyUsed_Withslm001_ReturnTrue)
{
    ASSERT_TRUE(controller->isMonsterAlreadyUsed("slm001"));
}

} // namespace mapeditor::controllers::editmonsterencounterformcontroller::unittest
