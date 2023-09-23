#include <gtest/gtest.h>
#include "monsterUtils.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <string>

using namespace commoneditor::ui;
using namespace thewarrior::models;

namespace commoneditor::ui::monsterutils::unittest {

TEST(MonsterUtils_getEncounterRatioStr, WithNormalReturnNormalStr)
{
    ASSERT_EQ("Normal", MonsterUtils::getEncounterRatioStr(MonsterEncounterRatio::Normal));
}

TEST(MonsterUtils_getEncounterRatioStr, WithLessThanNormalReturnLessThanNormalStr)
{
    ASSERT_EQ("Less than normal", MonsterUtils::getEncounterRatioStr(MonsterEncounterRatio::LessThanNormal));
}

TEST(MonsterUtils_getEncounterRatioStr, WithRareReturnRareStr)
{
    ASSERT_EQ("Rare", MonsterUtils::getEncounterRatioStr(MonsterEncounterRatio::Rare));
}

TEST(MonsterUtils_getEncounterRatioFromString, WithNormalStr_ReturnNormal)
{
    ASSERT_EQ(MonsterEncounterRatio::Normal, MonsterUtils::getEncounterRatioFromString("Normal"));
}

TEST(MonsterUtils_getEncounterRatioFromString, WithLessThanNormalStr_ReturnLessThanNormal)
{
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, MonsterUtils::getEncounterRatioFromString("Less than normal"));
}

TEST(MonsterUtils_getEncounterRatioFromString, WithRareStr_ReturnRare)
{
    ASSERT_EQ(MonsterEncounterRatio::Rare, MonsterUtils::getEncounterRatioFromString("Rare"));
}

TEST(MonsterUtils_getEncounterRatioFromString, WithRareStrAllCaps_ThrowInvalidArgument)
{
    try {
        MonsterUtils::getEncounterRatioFromString("RARE");
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("Encounter ratio RARE cannot be determined.", err.what());
    }
}

} // namespace commoneditor::ui::monsterutils::unittest
