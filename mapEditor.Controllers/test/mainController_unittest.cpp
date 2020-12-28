#include "mainController.hpp"
#include <gtest/gtest.h>

using namespace std;

class SampleMainController : public ::testing::Test
{
public:
    SampleMainController() {
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        auto &tile { map->getTileForEditing(0) };
        tile.setTextureName("tex1");
        tile.setTextureIndex(0);
        tile.setObjectTextureName("tex1");
        tile.setObjectTextureIndex(0);
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