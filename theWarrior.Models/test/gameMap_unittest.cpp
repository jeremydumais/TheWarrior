#include <gtest/gtest.h>
#include <algorithm>
#include "gameMap.hpp"
#include "monsterZone.hpp"
#include "rgbItemColor.hpp"

using thewarrior::models::GameMap;
using thewarrior::models::MonsterZone;
using thewarrior::models::Point;
using thewarrior::models::RGBItemColor;

class SampleGameMap5x6WithTwoTextures : public ::testing::Test {
 public:
    SampleGameMap5x6WithTwoTextures()
        : map(5, 6) {
        map.addTexture({
                "a", "a.png",
                512, 512,
                32, 32
                });

        map.addTexture({
                "b", "b.png",
                512, 512,
                32, 32
                });
        const auto &firstTexture = map.getTextures()[0];
        auto &tileIndex0 = map.getTileForEditing(0);
        tileIndex0.setTextureName(firstTexture.getName());
        tileIndex0.setTextureIndex(0);
        tileIndex0.setObjectTextureName(firstTexture.getName());
        tileIndex0.setObjectTextureIndex(0);
        map.addMonsterZone(MonsterZone("Zone1", RGBItemColor("Green", "#00FF00")));
        map.addMonsterZone(MonsterZone("Zone2", RGBItemColor("Blue", "#0000FF")));
    }
    GameMap map;
};

// Texture assignation                    MonsterZone Indexes
// N = Not assigned, A = Assigned
//      0 1 2 3 4 5                          0  1  2  3  4  5
//      -----------                          ----------------
//   0  N N N N N N                       0 -1 -1 -1 -1 -1 -1
//   6  N N A N N N                       6 -1 -1  0 -1 -1 -1
//  12  N A A A N N                      12 -1  1  1  0 -1 -1
//  18  N N A N N N                      18 -1 -1  0 -1 -1 -1
//  24  N N N N A N                      24 -1 -1 -1 -1  1 -1
//  30  N N N N N N                      30 -1 -1 -1 -1 -1 -1
class SampleGameMapWithTilesAssigned : public ::testing::Test {
 public:
    SampleGameMapWithTilesAssigned()
        : map(6, 6) {
            map.addTexture({
                    "tex1",
                    "tex1.png",
                    512, 256,
                    32, 32
                    });
            map.addMonsterZone(MonsterZone("Zone1", RGBItemColor("Green", "#00FF00")));
            map.addMonsterZone(MonsterZone("Zone2", RGBItemColor("Blue", "#0000FF")));
            auto &tile8 { map.getTileForEditing(8) };
            tile8.setTextureName("tex1");
            tile8.setTextureIndex(0);
            tile8.setCanPlayerSteppedOn(false);
            tile8.setMonsterZoneIndex(0);
            auto &tile13 { map.getTileForEditing(13) };
            tile13.setTextureIndex(0);
            tile13.setMonsterZoneIndex(1);
            auto &tile14 { map.getTileForEditing(14) };
            tile14.setTextureIndex(0);
            tile14.setMonsterZoneIndex(1);
            auto &tile15 { map.getTileForEditing(15) };
            tile15.setTextureIndex(1);
            tile15.setMonsterZoneIndex(0);
            auto &tile20 { map.getTileForEditing(20) };
            tile20.setObjectTextureIndex(1);
            tile20.setMonsterZoneIndex(0);
            auto &tile28 { map.getTileForEditing(28) };
            tile28.setObjectTextureName("tex1");
            tile28.setMonsterZoneIndex(1);
        }
    GameMap map;
};

class SampleGameMap5x6WithOneMonsterZone : public ::testing::Test {
 public:
    SampleGameMap5x6WithOneMonsterZone()
        : map(5, 6) {
        map.addMonsterZone(MonsterZone("Zone1", RGBItemColor("Green", "#00FF00")));
    }
    GameMap map;
};

TEST(GameMap_Constructor, ZeroWidth_ThrowInvalidArgument) {
    try {
        GameMap map(0, 10);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("width must be greater than zero.", err.what());
    }
}

TEST(GameMap_Constructor, ZeroHeight_ThrowInvalidArgument) {
    try {
        GameMap map(10, 0);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("height must be greater than zero.", err.what());
    }
}

TEST(GameMap_getWidth, WidthFive_ReturnFive) {
    GameMap map(5, 10);
    ASSERT_EQ(5, map.getWidth());
}

TEST(GameMap_getHeight, HeightFive_ReturnFive) {
    GameMap map(6, 5);
    ASSERT_EQ(5, map.getHeight());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileForEditing_WithIndexMinus1_ThrowInvalidArgument) {
    try {
        map.getTileForEditing(-1);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("index must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileForEditing_WithIndexMinus0_ReturnFirstTile) {
    const auto &tile { map.getTileForEditing(0) };
    ASSERT_EQ("a", tile.getTextureName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileForEditing_WithPointMinus1And0_ThrowInvalidArgument) {
    try {
        map.getTileForEditing({-1, 0});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("x must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileForEditing_WithPoint0AndMinus1_ThrowInvalidArgument) {
    try {
        map.getTileForEditing({0, -1});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("y must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileForEditing_WithPoint0And0_ReturnFirstTile) {
    const auto &tile { map.getTileForEditing({0 , 0}) };
    ASSERT_EQ("a", tile.getTextureName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTilesForEditing_WithEmpty_Return0Tiles) {
    const auto tiles = map.getTilesForEditing({ });
    ASSERT_EQ(0, tiles.size());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTilesForEditing_With0_Return1Tiles) {
    const auto tiles = map.getTilesForEditing({0});
    ASSERT_EQ(1, tiles.size());
    ASSERT_EQ("a", tiles.at(0)->getTextureName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTilesForEditing_WithMinus1_ReturnEmpty) {
    const auto tiles = map.getTilesForEditing({-1});
    ASSERT_EQ(0, tiles.size());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTilesForEditing_With0And1_Return2Tiles) {
    const auto tiles = map.getTilesForEditing({0, 1});
    ASSERT_EQ(2, tiles.size());
    ASSERT_EQ("a", tiles.at(0)->getTextureName());
}


TEST_F(SampleGameMap5x6WithTwoTextures, getTilesForEditing_With30_ReturnEmpty) {
    const auto tiles = map.getTilesForEditing({30});
    ASSERT_EQ(0, tiles.size());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileFromCoord_WithPointMinus1And0_ThrowInvalidArgument) {
    try {
        map.getTileFromCoord({-1, 0});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("x must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileFromCoord_WithPoint0AndMinus1_ThrowInvalidArgument) {
    try {
        map.getTileFromCoord({0, -1});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("y must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileFromCoord_WithPoint0And0_ReturnFirstTile) {
    const auto &tile { map.getTileFromCoord({0 , 0}) };
    ASSERT_EQ("a", tile.getTextureName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getCoordFromTileIndex_WithIndex0_Return0_0) {
    ASSERT_EQ(Point(0, 0), map.getCoordFromTileIndex(0));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getCoordFromTileIndex_WithIndex1_Return1_0) {
    ASSERT_EQ(Point(1, 0), map.getCoordFromTileIndex(1));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getCoordFromTileIndex_WithIndex4_Return4_0) {
    ASSERT_EQ(Point(4, 0), map.getCoordFromTileIndex(4));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getCoordFromTileIndex_WithIndex5_Return0_1) {
    ASSERT_EQ(Point(0, 1), map.getCoordFromTileIndex(5));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getCoordFromTileIndex_WithIndexMinus1_ThrowInvalidArgument) {
    try {
        map.getCoordFromTileIndex(-1);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("index must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPointMinus1And0_ThrowInvalidArgument) {
    try {
        map.getTileIndexFromCoord({-1, 0});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("x must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPoint0AndMinus1_ThrowInvalidArgument) {
    try {
        map.getTileIndexFromCoord({0, -1});
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("y must be a positive number", err.what());
    }
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPoint0_0_Return0) {
    ASSERT_EQ(0, map.getTileIndexFromCoord(Point(0, 0)));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPoint1_0_Return1) {
    ASSERT_EQ(1, map.getTileIndexFromCoord(Point(1, 0)));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPoint5_0_Return4) {
    ASSERT_EQ(4, map.getTileIndexFromCoord(Point(4, 0)));
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTileIndexFromCoord_WithPoint0_1_Return5) {
    ASSERT_EQ(5, map.getTileIndexFromCoord(Point(0, 1)));
}
//-------------------------------

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnLeft_ReturnFalse) {
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(-1, 0, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnLeft_ReturnTrue) {
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(-2, 0, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnTop_ReturnFalse) {
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, -1, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnTop_ReturnTrue) {
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, -2, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnRight_ReturnFalse) {
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, 0, -1, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnRight_ReturnTrue) {
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, 0, -2, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnBottom_ReturnFalse) {
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, 0, 0, -1));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnBottom_ReturnTrue) {
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, 0, 0, -2));
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnLeft_ReturnSuccess) {
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the first column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[0].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnLeft_ReturnSuccess) {
    map.resizeMap(-1, 0, 0, 0);
    ASSERT_EQ(5, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new first column
    ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[1][0].isAssigned());
    ASSERT_TRUE(map.getTiles()[2][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[3][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[4][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[5][0].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnLeft_ReturnSuccess) {
    map.resizeMap(-2, 0, 0, 0);
    ASSERT_EQ(4, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new first column
    ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
    ASSERT_TRUE(map.getTiles()[1][0].isAssigned());
    ASSERT_TRUE(map.getTiles()[2][0].isAssigned());
    ASSERT_TRUE(map.getTiles()[3][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[4][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[5][0].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnLeft_ReturnSuccess) {
    map.resizeMap(1, 0, 0, 0);
    ASSERT_EQ(7, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new first column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[0].isAssigned());
        ASSERT_FALSE(row[1].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusTwoOnLeft_ReturnSuccess) {
    map.resizeMap(2, 0, 0, 0);
    ASSERT_EQ(8, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new first column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[0].isAssigned());
        ASSERT_FALSE(row[1].isAssigned());
        ASSERT_FALSE(row[2].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnTop_ReturnSuccess) {
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the first row
    for (int i=0; i < map.getWidth(); i++) {
        ASSERT_FALSE(map.getTiles()[0][i].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnTop_ReturnSuccess) {
    map.resizeMap(0, -1, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(5, map.getHeight());
    // Check the new first row
    ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][1].isAssigned());
    ASSERT_TRUE(map.getTiles()[0][2].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][3].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][4].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnTop_ReturnSuccess) {
    map.resizeMap(0, -2, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(4, map.getHeight());
    // Check the new first row
    ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
    ASSERT_TRUE(map.getTiles()[0][1].isAssigned());
    ASSERT_TRUE(map.getTiles()[0][2].isAssigned());
    ASSERT_TRUE(map.getTiles()[0][3].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][4].isAssigned());
    ASSERT_FALSE(map.getTiles()[0][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnTop_ReturnSuccess) {
    map.resizeMap(0, 1, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(7, map.getHeight());
    // Check the new first row
    for (int i=0; i < map.getWidth(); i++) {
        ASSERT_FALSE(map.getTiles()[0][i].isAssigned());
        ASSERT_FALSE(map.getTiles()[1][i].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnRight_ReturnSuccess) {
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the last column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnRight_ReturnSuccess) {
    map.resizeMap(0, 0, -1, 0);
    ASSERT_EQ(5, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    size_t lastCol { map.getWidth()-1 };
    // Check the new last column
    ASSERT_FALSE(map.getTiles()[0][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[1][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[2][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[3][lastCol].isAssigned());
    ASSERT_TRUE(map.getTiles()[4][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[5][lastCol].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnRight_ReturnSuccess) {
    map.resizeMap(0, 0, -2, 0);
    ASSERT_EQ(4, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    size_t lastCol { map.getWidth()-1 };
    // Check the new last column
    ASSERT_FALSE(map.getTiles()[0][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[1][lastCol].isAssigned());
    ASSERT_TRUE(map.getTiles()[2][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[3][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[4][lastCol].isAssigned());
    ASSERT_FALSE(map.getTiles()[5][lastCol].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnRight_ReturnSuccess) {
    map.resizeMap(0, 0, 1, 0);
    ASSERT_EQ(7, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new last column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
        ASSERT_FALSE(row[map.getWidth()-2].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusTwoOnRight_ReturnSuccess) {
    map.resizeMap(0, 0, 2, 0);
    ASSERT_EQ(8, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the new last column
    for (const auto &row : map.getTiles()) {
        ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
        ASSERT_FALSE(row[map.getWidth()-2].isAssigned());
        ASSERT_FALSE(row[map.getWidth()-3].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnBottom_ReturnSuccess) {
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
    // Check the last row
    for (int i=0; i < map.getWidth(); i++) {
        ASSERT_FALSE(map.getTiles()[map.getHeight() - 1][i].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnBottom_ReturnSuccess) {
    map.resizeMap(0, 0, 0, -1);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(5, map.getHeight());
    size_t lastRow { map.getHeight() - 1 };
    // Check the new last row
    ASSERT_FALSE(map.getTiles()[lastRow][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][1].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][2].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][3].isAssigned());
    ASSERT_TRUE(map.getTiles()[lastRow][4].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnBottom_ReturnSuccess) {
    map.resizeMap(0, 0, 0, -2);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(4, map.getHeight());
    size_t lastRow { map.getHeight() - 1 };
    // Check the new last row
    ASSERT_FALSE(map.getTiles()[lastRow][0].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][1].isAssigned());
    ASSERT_TRUE(map.getTiles()[lastRow][2].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][3].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][4].isAssigned());
    ASSERT_FALSE(map.getTiles()[lastRow][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnBottom_ReturnSuccess) {
    map.resizeMap(0, 0, 0, 1);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(7, map.getHeight());
    // Check the new last row
    for (int i=0; i < map.getWidth(); i++) {
        ASSERT_FALSE(map.getTiles()[map.getHeight() - 1][i].isAssigned());
        ASSERT_FALSE(map.getTiles()[map.getHeight() - 2][i].isAssigned());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetLeftNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument) {
    try {
        map.resizeMap(-6, 0, 0, 0);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("This left offset would remove all the remaining tiles.", err.what());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetTopNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument) {
    try {
        map.resizeMap(0, -6, 0, 0);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("This top offset would remove all the remaining tiles.", err.what());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetRightNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument) {
    try {
        map.resizeMap(0, 0, -6, 0);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("This right offset would remove all the remaining tiles.", err.what());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetBottomNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument) {
    try {
        map.resizeMap(0, 0, 0, -6);
        FAIL();
    }
    catch(std::invalid_argument &err) {
        ASSERT_STREQ("This bottom offset would remove all the remaining tiles.", err.what());
    }
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPoint2And2_ReturnTrue) {
    ASSERT_TRUE(map.canSteppedOnTile({2, 2}));
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPoint2And2_ReturnFalse) {
    ASSERT_FALSE(map.canSteppedOnTile({2, 1}));
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPointMinus1And2_ReturnFalse) {
    ASSERT_FALSE(map.canSteppedOnTile({-1, 2}));
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPoint2AndMinus1_ReturnFalse) {
    ASSERT_FALSE(map.canSteppedOnTile({2, -1}));
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPoint6And1_ReturnFalse) {
    ASSERT_FALSE(map.canSteppedOnTile({6, 1}));
}

TEST_F(SampleGameMapWithTilesAssigned, canSteppedOnTile_WithPoint1And6_ReturnFalse) {
    ASSERT_FALSE(map.canSteppedOnTile({1, 6}));
}

TEST(GameMap_getMonsterZones, withEmptyZone_ReturnEmpty) {
    GameMap map(6, 5);
    ASSERT_EQ(0, map.getMonsterZones().size());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getMonsterZones_ReturnTwoZones) {
    const auto &zones = map.getMonsterZones();
    ASSERT_EQ(2, zones.size());
    ASSERT_EQ("Zone1", zones[0].getName());
    ASSERT_EQ("Zone2", zones[1].getName());
}

TEST(GameMap_getMonsterZoneByName, withEmptyName_ReturnNullOpt) {
    GameMap map(6, 5);
    const auto zone = map.getMonsterZoneByName("");
    ASSERT_FALSE(zone.has_value());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getMonsterZoneByName_WithZone1_ReturnZone) {
    const auto zone = map.getMonsterZoneByName("Zone1");
    ASSERT_TRUE(zone.has_value());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getMonsterZoneByName_WithZone1Caps_ReturnZone) {
    const auto zone = map.getMonsterZoneByName("ZONE1");
    ASSERT_TRUE(zone.has_value());
    ASSERT_EQ("Zone1", zone->get().getName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getMonsterZoneByName_WithZone2_ReturnNone) {
    const auto zone = map.getMonsterZoneByName("Zone3");
    ASSERT_FALSE(zone.has_value());
}

TEST_F(SampleGameMap5x6WithTwoTextures, addMonsterZone_WithNotExistingMonsterZone_ReturnTrue) {
    const MonsterZone zone("Test", RGBItemColor("Green", "#00FF00"));
    ASSERT_TRUE(map.addMonsterZone(zone));
    ASSERT_EQ(3, map.getMonsterZones().size());
}

TEST_F(SampleGameMap5x6WithTwoTextures, addMonsterZone_WithExistingMonsterZone_ReturnFalse) {
    const MonsterZone zoneSameName("Zone1", RGBItemColor("Green", "#00FF00"));
    ASSERT_FALSE(map.addMonsterZone(zoneSameName));
    ASSERT_EQ("The zone Zone1 already exist.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithOneMonsterZone, addMonsterZone_WithOnlyOneMSToTrue_ReturnTrueAndSetOnlyOneMSToFalse) {
    const MonsterZone zoneSameName("Zone3", RGBItemColor("Green", "#00FF00"));
    map.setUseOnlyOneMonsterZone(true);
    ASSERT_TRUE(map.addMonsterZone(zoneSameName));
    ASSERT_FALSE(map.useOnlyOneMonsterZone());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceMonsterZone_WithNonExistingOldZone_ReturnFalse) {
    const MonsterZone zoneSameName("Zone1", RGBItemColor("Green", "#00FF00"));
    ASSERT_FALSE(map.replaceMonsterZone("Zone99", zoneSameName));
    ASSERT_EQ("Unable to find the zone Zone99 to update.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceMonsterZone_WithExistingMonsterZone_ReturnFalse) {
    const MonsterZone zoneSameName("Zone2", RGBItemColor("Green", "#00FF00"));
    ASSERT_FALSE(map.replaceMonsterZone("Zone1", zoneSameName));
    ASSERT_EQ("The zone Zone2 already exist.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceMonsterZone_WithSameNameMonsterZone_ReturnFalse) {
    const MonsterZone zoneSameName("Zone1", RGBItemColor("Red", "#FF0000"));
    ASSERT_TRUE(map.replaceMonsterZone("Zone1", zoneSameName));
    const auto zones = map.getMonsterZones();
    ASSERT_EQ(2, zones.size());
    ASSERT_EQ("Zone1", zones[0].getName());
    ASSERT_EQ("Red", zones[0].getColor().getName());
    ASSERT_EQ("Zone2", zones[1].getName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceMonsterZone_WithDiffNameMonsterZone_ReturnFalse) {
    const MonsterZone zoneSameName("Zone3", RGBItemColor("Red", "#FF0000"));
    ASSERT_TRUE(map.replaceMonsterZone("Zone1", zoneSameName));
    const auto zones = map.getMonsterZones();
    ASSERT_EQ(2, zones.size());
    ASSERT_EQ("Zone3", zones[0].getName());
    ASSERT_EQ("Red", zones[0].getColor().getName());
    ASSERT_EQ("Zone2", zones[1].getName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, removeMonsterZone_WithNonExistingMonsterZone_ReturnFalse) {
    ASSERT_FALSE(map.removeMonsterZone("Zone3"));
    ASSERT_EQ("Unable to find the zone Zone3 to delete.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, removeMonsterZone_WithZone1_ReturnTrue) {
    ASSERT_TRUE(map.removeMonsterZone("Zone1"));
    const auto &zones = map.getMonsterZones();
    ASSERT_EQ(1, zones.size());
    ASSERT_EQ("Zone2", zones[0].getName());
}

TEST_F(SampleGameMap5x6WithTwoTextures, removeMonsterZone_WithZone2_ReturnTrue) {
    ASSERT_TRUE(map.removeMonsterZone("Zone2"));
    const auto &zones = map.getMonsterZones();
    ASSERT_EQ(1, zones.size());
    ASSERT_EQ("Zone1", zones[0].getName());
}

TEST_F(SampleGameMap5x6WithOneMonsterZone, removeMonsterZone_WithOnlyOneMSToTrue_ReturnTrueAndSetOnlyOneMSToFalse) {
    map.setUseOnlyOneMonsterZone(true);
    ASSERT_TRUE(map.removeMonsterZone("Zone1"));
    ASSERT_FALSE(map.useOnlyOneMonsterZone());
}

TEST(GameMap_useOnlyOneMonsterZone, withDefaultConstructor_ReturnFalse) {
    GameMap map(5, 6);
    ASSERT_FALSE(map.useOnlyOneMonsterZone());
}

TEST_F(SampleGameMap5x6WithOneMonsterZone, setUseOnlyOneMonsterZone_withTrueOnOneMonsterZone_ReturnTrue) {
    ASSERT_TRUE(map.setUseOnlyOneMonsterZone(true));
    ASSERT_TRUE(map.useOnlyOneMonsterZone());
}

TEST(GameMap_setUseOnlyOneMonsterZone, withTrueOnNoMonsterZone_ReturnFalse) {
    GameMap map(5, 6);
    ASSERT_FALSE(map.setUseOnlyOneMonsterZone(true));
    ASSERT_EQ("You must have exactly one monster zone.", map.getLastError());
    ASSERT_FALSE(map.useOnlyOneMonsterZone());
}

TEST_F(SampleGameMap5x6WithTwoTextures, withTrueOnTwoMonsterZones_ReturnFalse) {
    ASSERT_FALSE(map.setUseOnlyOneMonsterZone(true));
    ASSERT_EQ("You must have exactly one monster zone.", map.getLastError());
    ASSERT_FALSE(map.useOnlyOneMonsterZone());
}

TEST_F(SampleGameMapWithTilesAssigned, unassignMonsterZoneOnAllTiles_withZone0_ReturnSuccess) {
    map.unassignMonsterZoneOnAllTiles(0);
    ASSERT_EQ(-1, map.getTileForEditing(8).getMonsterZoneIndex());
    ASSERT_EQ(-1, map.getTileForEditing(15).getMonsterZoneIndex());
    ASSERT_EQ(-1, map.getTileForEditing(20).getMonsterZoneIndex());
    // Check that the others did not changed
    ASSERT_EQ(1, map.getTileForEditing(13).getMonsterZoneIndex());
    ASSERT_EQ(1, map.getTileForEditing(14).getMonsterZoneIndex());
    ASSERT_EQ(1, map.getTileForEditing(28).getMonsterZoneIndex());
}

TEST_F(SampleGameMapWithTilesAssigned, unassignMonsterZoneOnAllTiles_withZone1_ReturnSuccess) {
    map.unassignMonsterZoneOnAllTiles(1);
    ASSERT_EQ(-1, map.getTileForEditing(13).getMonsterZoneIndex());
    ASSERT_EQ(-1, map.getTileForEditing(14).getMonsterZoneIndex());
    ASSERT_EQ(-1, map.getTileForEditing(28).getMonsterZoneIndex());
    // Check that the others did not changed
    ASSERT_EQ(0, map.getTileForEditing(8).getMonsterZoneIndex());
    ASSERT_EQ(0, map.getTileForEditing(15).getMonsterZoneIndex());
    ASSERT_EQ(0, map.getTileForEditing(20).getMonsterZoneIndex());
}

TEST_F(SampleGameMapWithTilesAssigned, unassignMonsterZoneOnAllTiles_withZone2_ReturnSuccess) {
    map.unassignMonsterZoneOnAllTiles(2);
    ASSERT_EQ(0, map.getTileForEditing(8).getMonsterZoneIndex());
    ASSERT_EQ(0, map.getTileForEditing(15).getMonsterZoneIndex());
    ASSERT_EQ(0, map.getTileForEditing(20).getMonsterZoneIndex());
    ASSERT_EQ(1, map.getTileForEditing(13).getMonsterZoneIndex());
    ASSERT_EQ(1, map.getTileForEditing(14).getMonsterZoneIndex());
    ASSERT_EQ(1, map.getTileForEditing(28).getMonsterZoneIndex());
}
