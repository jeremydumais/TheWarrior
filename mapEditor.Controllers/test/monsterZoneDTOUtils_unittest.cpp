#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>
#include "monsterZone.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "rgbItemColor.hpp"

using thewarrior::models::MonsterZone;
using thewarrior::models::MonsterZoneMonsterEncounter;
using thewarrior::models::MonsterEncounterRatio;
using thewarrior::models::RGBItemColor;

namespace mapeditor::controllers::monsterzonedtoutils::unittest {

void assertMonsterZoneDTO(MonsterZoneDTO expected, MonsterZoneDTO actual) {
    ASSERT_EQ(actual.m_name, expected.m_name);
    ASSERT_EQ(actual.m_colorName, expected.m_colorName);
    ASSERT_EQ(actual.m_colorValue, expected.m_colorValue);
    ASSERT_EQ(actual.m_ratioEncounter, expected.m_ratioEncounter);
    ASSERT_EQ(actual.m_ratioEncounterOn, expected.m_ratioEncounterOn);
    ASSERT_EQ(actual.m_monsterEncounters, expected.m_monsterEncounters);
}

TEST(monsterZoneDTOUtils_fromMonsterZone, withEmptyZoneEncounter_ReturnValidDTO) {
    MonsterZone zone("Test", RGBItemColor("black", "#000000"), 1, 2, { });
    auto actual = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, { }
    };
    assertMonsterZoneDTO(actual, MonsterZoneDTOUtils::fromMonsterZone(zone));
}

TEST(monsterZoneDTOUtils_fromMonsterZone, withOneZoneEncounter_ReturnValidDTO) {
    MonsterZone zone("Test", RGBItemColor("black", "#000000"), 1, 2, {
            MonsterZoneMonsterEncounter("DRA001", MonsterEncounterRatio::Normal) });
    const auto actual = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, { { "DRA001", "Normal" } }
    };
    assertMonsterZoneDTO(actual, MonsterZoneDTOUtils::fromMonsterZone(zone));
}

TEST(monsterZoneDTOUtils_fromMonsterZone, withThreeZonesEncounter_ReturnValidDTO) {
    MonsterZone zone("Test", RGBItemColor("black", "#000000"), 1, 2, {
            MonsterZoneMonsterEncounter("DRA001", MonsterEncounterRatio::LessThanNormal),
            MonsterZoneMonsterEncounter("DRA002", MonsterEncounterRatio::Normal),
            MonsterZoneMonsterEncounter("DRA003", MonsterEncounterRatio::Rare) });
    const auto expected = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, {
            { "DRA001", "Less than normal" },
            { "DRA002", "Normal" },
            { "DRA003", "Rare" },
        }
    };
    assertMonsterZoneDTO(expected, MonsterZoneDTOUtils::fromMonsterZone(zone));
}

TEST(monsterZoneDTOUtils_fromMonsterZoneMonsterEncounter, WithDRA001_Rare_ReturnPairString) {
    MonsterZoneMonsterEncounter encounter("DRA001", MonsterEncounterRatio::Rare);
    std::pair<std::string, std::string> expected = { "DRA001", "Rare"};
    ASSERT_EQ(expected, MonsterZoneDTOUtils::fromMonsterZoneMonsterEncounter(encounter));
}

TEST(monsterZoneDTOUtils_fromMonsterZoneMonsterEncounter, WithDRA001_LessThanNormal_ReturnPairString) {
    MonsterZoneMonsterEncounter encounter("DRA001", MonsterEncounterRatio::LessThanNormal);
    std::pair<std::string, std::string> expected = { "DRA001", "Less than normal"};
    ASSERT_EQ(expected, MonsterZoneDTOUtils::fromMonsterZoneMonsterEncounter(encounter));
}

TEST(monsterZoneDTOUtils_fromMonsterZoneMonsterEncounter, WithDRA001_Normal_ReturnPairString) {
    MonsterZoneMonsterEncounter encounter("DRA001", MonsterEncounterRatio::Normal);
    std::pair<std::string, std::string> expected = { "DRA001", "Normal"};
    ASSERT_EQ(expected, MonsterZoneDTOUtils::fromMonsterZoneMonsterEncounter(encounter));
}

TEST(monsterZoneDTOUtils_toMonsterZone, withEmptyZoneEncouter_ReturnValidZone) {
    MonsterZone expected("Test", RGBItemColor("black", "#000000"), 1, 2, { });
    auto dto = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, { }
    };
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZone(dto));
}

TEST(monsterZoneDTOUtils_toMonsterZone, withOneZoneEncouter_ReturnValidZone) {
    MonsterZone expected("Test", RGBItemColor("black", "#000000"), 1, 2, {
            MonsterZoneMonsterEncounter("DRA001", MonsterEncounterRatio::Normal)
            });
    auto dto = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, { { "DRA001", "Normal" } }
    };
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZone(dto));
}

TEST(monsterZoneDTOUtils_toMonsterZone, withThreeZonesEncouter_ReturnValidZone) {
    MonsterZone expected("Test", RGBItemColor("black", "#000000"), 1, 2, {
            MonsterZoneMonsterEncounter("DRA001", MonsterEncounterRatio::LessThanNormal),
            MonsterZoneMonsterEncounter("DRA002", MonsterEncounterRatio::Normal),
            MonsterZoneMonsterEncounter("DRA003", MonsterEncounterRatio::Rare)
            });
    auto dto = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, {
            { "DRA001", "Less than normal" },
            { "DRA002", "Normal" },
            { "DRA003", "Rare" }
        }
    };
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZone(dto));
}

TEST(monsterZoneDTOUtils_toMonsterZone, withUnknownRatio_ThrowInvalidArgument) {
    auto dto = MonsterZoneDTO {
        "Test", "black", "#000000", 1, 2, { { "DRA001", "Normal1" } }
    };
    try {
        MonsterZoneDTOUtils::toMonsterZone(dto);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
       ASSERT_STREQ("unknown monster encouter ratio", err.what());
    }
}

TEST(monsterZoneDTOUtils_toMonsterZoneMonsterEncounter, withDRA001_Rare_ReturnEncounter) {
    std::pair<std::string, std::string> dto = { "DRA001", "Rare" };
    MonsterZoneMonsterEncounter expected("DRA001", MonsterEncounterRatio::Rare);
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZoneMonsterEncounter(dto));
}

TEST(monsterZoneDTOUtils_toMonsterZoneMonsterEncounter, withDRA001_LessThanNormal_ReturnEncounter) {
    std::pair<std::string, std::string> dto = { "DRA001", "Less than normal" };
    MonsterZoneMonsterEncounter expected("DRA001", MonsterEncounterRatio::LessThanNormal);
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZoneMonsterEncounter(dto));
}

TEST(monsterZoneDTOUtils_toMonsterZoneMonsterEncounter, withDRA001_Normal_ReturnEncounter) {
    std::pair<std::string, std::string> dto = { "DRA001", "Normal" };
    MonsterZoneMonsterEncounter expected("DRA001", MonsterEncounterRatio::Normal);
    ASSERT_EQ(expected, MonsterZoneDTOUtils::toMonsterZoneMonsterEncounter(dto));
}

}  // namespace mapeditor::controllers::monsterzonedtoutils::unittest
