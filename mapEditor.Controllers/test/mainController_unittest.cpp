#include "mainController.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace mapeditor::controllers;

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

TEST(MainController_Constructor, Default_ReturnSuccess)
{
    MainController mainController;
    ASSERT_EQ(nullptr, mainController.getMap());
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
