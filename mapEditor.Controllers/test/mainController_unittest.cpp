#include "mainController.hpp"
#include <gtest/gtest.h>

using namespace std;

class SampleMainController : public ::testing::Test
{
public:
    SampleMainController() {
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
    }
	MainController mainController;
};

// N = Not assigned, A = Assigned
//      0 1 2 3 4 5
//      -----------
//   0  N N N N N N
//   6  N N A N N N
//  12  N A A A N N
//  18  N N A N N N
//  24  N N N N A N
//  30  N N N N N N
class SampleMainControllerWithTilesAssigned : public ::testing::Test
{
public:
    SampleMainControllerWithTilesAssigned() {
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
        auto &tile8 { map->getTileForEditing(8) };
        tile8.setTextureName("tex1");
        tile8.setTextureIndex(0); 
        auto &tile13 { map->getTileForEditing(13) };
        tile13.setTextureIndex(0); 
        auto &tile14 { map->getTileForEditing(14) };
        tile14.setTextureIndex(0); 
        auto &tile15 { map->getTileForEditing(15) };
        tile15.setTextureIndex(1); 
        auto &tile20 { map->getTileForEditing(20) };
        tile20.setObjectTextureIndex(1); 
        auto &tile28 { map->getTileForEditing(28) };
        tile28.setObjectTextureName("tex1"); 
    }
	MainController mainController;
};


TEST(MainController_Constructor, Default_ReturnSuccess)
{
    MainController mainController;
    ASSERT_EQ(nullptr, mainController.getMap());
}

TEST(MainController_getAlreadyUsedTextureNames, DefaultConstructor_ReturnEmptyVector)
{
    MainController mainController;
    ASSERT_EQ(0, mainController.getAlreadyUsedTextureNames().size());
}

TEST_F(SampleMainController, getAlreadyUsedTextureNames_WithTwoTexture_ReturnVectorSizeTeo)
{
    ASSERT_EQ(2, mainController.getAlreadyUsedTextureNames().size());
}

TEST_F(SampleMainController, getTextureIndexFromPosition_WithSample1_ReturnValidPosition)
{
    auto actual = mainController.getTextureIndexFromPosition(Point(0, 255), mainController.getMap()->getTextureByName("tex1").get());
    ASSERT_EQ(0, actual);
}

TEST_F(SampleMainController, getTextureIndexFromPosition_WithSample2_ReturnValidPosition)
{
    auto actual = mainController.getTextureIndexFromPosition(Point(32, 255), mainController.getMap()->getTextureByName("tex1").get());
    ASSERT_EQ(1, actual);
}

TEST(MainController_createMap, WithInvalidWidth_ReturnFalse)
{
    MainController mainController;
    ASSERT_FALSE(mainController.createMap(0, 10));
    ASSERT_EQ("width must be greater than zero.", mainController.getLastError());
}

TEST_F(SampleMainController, addTexture_WithValidArg_ReturnTrue)
{
    ASSERT_TRUE(mainController.addTexture({
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ(3, mainController.getTextures().size());
}

TEST_F(SampleMainController, addTexture_WithInvalidArg_ReturnFalse)
{
    ASSERT_FALSE(mainController.addTexture({
            "",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ("name cannot be null or empty.", mainController.getLastError());
}

TEST_F(SampleMainController, replaceTexture_WithNonExistantName_ReturnFalse)
{
    ASSERT_FALSE(mainController.replaceTexture("x", {
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
    ASSERT_EQ("Unable to find the texture x in the texture list.", mainController.getLastError());
}

TEST_F(SampleMainController, replaceTexture_WithValidArgs_ReturnTrue)
{
    ASSERT_TRUE(mainController.replaceTexture("tex1", {
            "tex3",
            "tex3.png",
            512, 256,
            32, 32
        }));
}

TEST_F(SampleMainController, removeTexture_WithNonExistantName_ReturnFalse)
{
    ASSERT_FALSE(mainController.removeTexture("x"));
    ASSERT_EQ("Unable to find the texture x in the texture list.", mainController.getLastError());
}

TEST_F(SampleMainController, removeTexture_WithExistingName_ReturnTrue)
{
    ASSERT_TRUE(mainController.removeTexture("tex1"));
}

TEST_F(SampleMainController, isTextureUsedInMap_WithUnusedTexture_ReturnFalse)
{
    ASSERT_FALSE(mainController.isTextureUsedInMap("x"));
}

TEST_F(SampleMainController, isTextureUsedInMap_WithUsedTexture_ReturnTrue)
{
    ASSERT_TRUE(mainController.isTextureUsedInMap("tex1"));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnLeft_ReturnFalse)
{
    ASSERT_FALSE(mainController.isShrinkMapImpactAssignedTiles(-1, 0, 0, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnLeft_ReturnTrue)
{
    ASSERT_TRUE(mainController.isShrinkMapImpactAssignedTiles(-2, 0, 0, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnTop_ReturnFalse)
{
    ASSERT_FALSE(mainController.isShrinkMapImpactAssignedTiles(0, -1, 0, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnTop_ReturnTrue)
{
    ASSERT_TRUE(mainController.isShrinkMapImpactAssignedTiles(0, -2, 0, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnRight_ReturnFalse)
{
    ASSERT_FALSE(mainController.isShrinkMapImpactAssignedTiles(0, 0, -1, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnRight_ReturnTrue)
{
    ASSERT_TRUE(mainController.isShrinkMapImpactAssignedTiles(0, 0, -2, 0));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnBottom_ReturnFalse)
{
    ASSERT_FALSE(mainController.isShrinkMapImpactAssignedTiles(0, 0, 0, -1));
}

TEST_F(SampleMainControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnBottom_ReturnTrue)
{
    ASSERT_TRUE(mainController.isShrinkMapImpactAssignedTiles(0, 0, 0, -2));
}

/*TEST_F(SampleMainControllerWithTilesAssigned, resizeMap_WithMinusOneOnLeft_ReturnSuccess)
{
    mainController.resizeMap(-1, 0, 0, 0);
    ASSERT_EQ(5, mainController.getMap()->getWidth());
    ASSERT_EQ(6, mainController.getMap()->getHeight());
}*/

TEST_F(SampleMainController, replaceTilesTextureName_WithOneTileAffected_ReturnSuccess)
{
    mainController.replaceTilesTextureName("tex1", "newTex1");
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