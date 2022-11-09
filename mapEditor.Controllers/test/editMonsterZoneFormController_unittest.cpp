#include <gtest/gtest.h>
#include "editMonsterZoneFormController.hpp"
#include "containerOfMonsterStoreTestUtils.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <stdexcept>

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

TEST(EditMonsterZoneFormController_getEncounterRatioStr, WithNormalReturnNormalStr)
{
    ASSERT_EQ("Normal", EditMonsterZoneFormController::getEncounterRatioStr(MonsterEncounterRatio::Normal));
}

TEST(EditMonsterZoneFormController_getEncounterRatioStr, WithLessThanNormalReturnLessThanNormalStr)
{
    ASSERT_EQ("Less than normal", EditMonsterZoneFormController::getEncounterRatioStr(MonsterEncounterRatio::LessThanNormal));
}

TEST(EditMonsterZoneFormController_getEncounterRatioStr, WithRareReturnRareStr)
{
    ASSERT_EQ("Rare", EditMonsterZoneFormController::getEncounterRatioStr(MonsterEncounterRatio::Rare));
}

TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithNormalStr_ReturnNormal)
{
    ASSERT_EQ(MonsterEncounterRatio::Normal, EditMonsterZoneFormController::getEncounterRatioFromString("Normal"));
}

TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithLessThanNormalStr_ReturnLessThanNormal)
{
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, EditMonsterZoneFormController::getEncounterRatioFromString("Less than normal"));
}

TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithRareStr_ReturnRare)
{
    ASSERT_EQ(MonsterEncounterRatio::Rare, EditMonsterZoneFormController::getEncounterRatioFromString("Rare"));
}

TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithRareStrAllCaps_ThrowInvalidArgument)
{
    try {
        EditMonsterZoneFormController::getEncounterRatioFromString("RARE");
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("Encounter ratio RARE cannot be determined.", err.what());
    }
}

//TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithLessThanNormalReturnLessThanNormalStr)
//{
    //ASSERT_EQ("Less than normal", EditMonsterZoneFormController::getEncounterRatioStr(MonsterEncounterRatio::LessThanNormal));
//}

//TEST(EditMonsterZoneFormController_getEncounterRatioFromString, WithRareReturnRareStr)
//{
    //ASSERT_EQ("Rare", EditMonsterZoneFormController::getEncounterRatioStr(MonsterEncounterRatio::Rare));
//}

} // namespace mapeditor::controllers::editmonsterzoneformcontroller
