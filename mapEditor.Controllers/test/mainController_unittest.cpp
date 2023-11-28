#include <gtest/gtest.h>
#include <stdexcept>
#include "glComponentController.hpp"
#include "mainController.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include "rgbItemColor.hpp"

using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MainController;
using thewarrior::models::MonsterEncounterRatio;
using thewarrior::models::MonsterZone;
using thewarrior::models::MonsterZoneMonsterEncounter;
using thewarrior::models::RGBItemColor;

class SampleMainController : public ::testing::Test {
 public:
    SampleMainController() {
        mainController.setGLComponentController(&glController);
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
        map->addTexture({
            "tex2",
            "tex2.png",
            1024, 512,
            32, 32
        });
        auto &tile { map->getTileForEditing(0) };
        tile.setTextureName("tex1");
        tile.setTextureIndex(0);
        tile.setObjectTextureName("tex1");
        tile.setObjectTextureIndex(0);
        map->addMonsterZone(MonsterZone("Zone1", RGBItemColor("Black", "#000000"),
                1, 3,
                { MonsterZoneMonsterEncounter("DRA001", MonsterEncounterRatio::Normal) }));
        map->addMonsterZone(MonsterZone("Zone2", RGBItemColor("Blue", "#0000FF"),
                2, 4,
                { MonsterZoneMonsterEncounter("DRA002", MonsterEncounterRatio::Rare) }));
    }
    MainController mainController;
    GLComponentController glController;
};

// TODO: Transfert tests in GLComponentController
TEST(MainController_Constructor, Default_ReturnSuccess) {
    MainController mainController;
    GLComponentController glController;
    mainController.setGLComponentController(&glController);
    ASSERT_EQ(nullptr, mainController.getMap());
}

TEST(MainController_createMap, WithInvalidWidth_ReturnFalse) {
    MainController mainController;
    GLComponentController glController;
    mainController.setGLComponentController(&glController);
    ASSERT_FALSE(mainController.createMap(0, 10));
    ASSERT_EQ("width must be greater than zero.", mainController.getLastError());
}

TEST_F(SampleMainController, addTexture_WithValidArg_ReturnTrue) {
    ASSERT_TRUE(mainController.addTexture({
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ(3, mainController.getTextures().size());
}

TEST_F(SampleMainController, addTexture_WithInvalidArg_ReturnFalse) {
    ASSERT_FALSE(mainController.addTexture({
            "",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ("name cannot be null or empty.", mainController.getLastError());
}

TEST_F(SampleMainController, replaceTexture_WithNonExistantName_ReturnFalse) {
    ASSERT_FALSE(mainController.replaceTexture("x", {
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ("Unable to find the texture x in the texture list.", mainController.getLastError());
}

TEST_F(SampleMainController, replaceTexture_WithValidArgs_ReturnTrue) {
    ASSERT_TRUE(mainController.replaceTexture("tex1", {
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
}

TEST_F(SampleMainController, replaceTiles_WithDiffNameOneTileAffected_ReturnSuccess) {
    mainController.replaceTexture("tex1", {
            "newTex1",
            "tex1.png",
            512, 256,
            32, 32
        });
    auto map { mainController.getMap() };
    const auto &tileIndex0 = map->getTiles()[0][0];
    ASSERT_EQ("newTex1", tileIndex0.getTextureName());
    ASSERT_EQ(0, tileIndex0.getTextureIndex());
    ASSERT_EQ("newTex1", tileIndex0.getObjectTextureName());
    ASSERT_EQ(0, tileIndex0.getObjectTextureIndex());

    const auto &tileIndex1 = map->getTiles()[0][1];
    ASSERT_EQ("", tileIndex1.getTextureName());
    ASSERT_EQ(-1, tileIndex1.getTextureIndex());
    ASSERT_EQ("", tileIndex1.getObjectTextureName());
    ASSERT_EQ(-1, tileIndex1.getObjectTextureIndex());
}

TEST_F(SampleMainController, removeTexture_WithNonExistantName_ReturnFalse) {
    ASSERT_FALSE(mainController.removeTexture("x"));
    ASSERT_EQ("Unable to find the texture x in the texture list.", mainController.getLastError());
}

TEST_F(SampleMainController, removeTexture_WithExistingName_ReturnTrue) {
    ASSERT_TRUE(mainController.removeTexture("tex1"));
}

TEST_F(SampleMainController, addMonsterZone_WithInvalidArg_ReturnFalse) {
    ASSERT_FALSE(mainController.addMonsterZone({
            .m_name = "",
            .m_colorName = "Black",
            .m_colorValue = "#000000",
            .m_ratioEncounter = 3,
            .m_ratioEncounterOn = 5,
            .m_monsterEncounters = { { "DRA003", "Less than normal" } }
        }));
}

TEST_F(SampleMainController, addMonsterZone_WithExistingName_ReturnFalse) {
    ASSERT_FALSE(mainController.addMonsterZone({
            .m_name = "Zone1",
            .m_colorName = "Black",
            .m_colorValue = "#000000",
            .m_ratioEncounter = 3,
            .m_ratioEncounterOn = 5,
            .m_monsterEncounters = { { "DRA003", "Less than normal" } }
        }));
    ASSERT_EQ("The zone Zone1 already exist.", mainController.getLastError());
}

TEST_F(SampleMainController, addMonsterZone_WithExistingNameCAPS_ReturnFalse) {
    ASSERT_FALSE(mainController.addMonsterZone({
            .m_name = "ZONE1",
            .m_colorName = "Black",
            .m_colorValue = "#000000",
            .m_ratioEncounter = 3,
            .m_ratioEncounterOn = 5,
            .m_monsterEncounters = { { "DRA003", "Less than normal" } }
        }));
    ASSERT_EQ("The zone ZONE1 already exist.", mainController.getLastError());
}

TEST_F(SampleMainController, addMonsterZone_WithNonExistingName_ReturnTrue) {
    ASSERT_TRUE(mainController.addMonsterZone({
            .m_name = "Zone3",
            .m_colorName = "Black",
            .m_colorValue = "#000000",
            .m_ratioEncounter = 3,
            .m_ratioEncounterOn = 5,
            .m_monsterEncounters = { { "DRA003", "Less than normal" } }
        }));
    ASSERT_EQ(3, mainController.getMap()->getMonsterZones().size());
}

TEST_F(SampleMainController, replaceMonsterZone_WithInvalidName_ReturnFalse) {
    ASSERT_FALSE(mainController.replaceMonsterZone("Zone1", {
                .m_name = "",
                .m_colorName = "Black",
                .m_colorValue = "#000000",
                .m_ratioEncounter = 3,
                .m_ratioEncounterOn = 5,
                .m_monsterEncounters = { { "DRA003", "Less than normal" } }
                }));
}

TEST_F(SampleMainController, replaceMonsterZone_WithAlreadyExistingZone_ReturnFalse) {
    ASSERT_FALSE(mainController.replaceMonsterZone("Zone1", {
                .m_name = "Zone2",
                .m_colorName = "Black",
                .m_colorValue = "#000000",
                .m_ratioEncounter = 3,
                .m_ratioEncounterOn = 5,
                .m_monsterEncounters = { { "DRA003", "Less than normal" } }
                }));
}

TEST_F(SampleMainController, replaceMonsterZone_WithSameNameZone_ReturnTrue) {
    ASSERT_TRUE(mainController.replaceMonsterZone("Zone1", {
                .m_name = "Zone1",
                .m_colorName = "Black",
                .m_colorValue = "#000000",
                .m_ratioEncounter = 3,
                .m_ratioEncounterOn = 5,
                .m_monsterEncounters = { { "DRA003", "Less than normal" } }
                }));
}

TEST_F(SampleMainController, replaceMonsterZone_WithNewNameZone_ReturnTrue) {
    ASSERT_TRUE(mainController.replaceMonsterZone("Zone1", {
                .m_name = "Zone3",
                .m_colorName = "Black",
                .m_colorValue = "#000000",
                .m_ratioEncounter = 3,
                .m_ratioEncounterOn = 5,
                .m_monsterEncounters = { { "DRA003", "Less than normal" } }
                }));
}
