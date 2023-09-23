#include "manageTexturesController.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace monstereditor::controllers;
using namespace thewarrior::models;

class MapTextureControllerWith2Textures : public ::testing::Test
{
public:
	MapTextureControllerWith2Textures()
        : controller(textureContainer)
	{
		textureContainer.addTexture({
            "tex1", "tex1.png",
            256, 128,
            32, 16
        });
        textureContainer.addTexture({
            "tex2", "tex2.png",
            128, 128,
            16, 16
        });
	}
	ManageTextureController controller;
private:
    TextureContainer textureContainer;
};

TEST_F(MapTextureControllerWith2Textures, getTextures_Return2Textures)
{
    auto actual = controller.getTexturesNames();
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("tex1", actual.at(0));
    ASSERT_EQ("tex2", actual.at(1));
}

TEST_F(MapTextureControllerWith2Textures, getTextureByName_WithEmptyName_ReturnNull)
{
    ASSERT_EQ(nullptr, controller.getTextureByName(""));
}

TEST_F(MapTextureControllerWith2Textures, getTextureByName_WithWhiteSpacesName_ReturnNull)
{
    ASSERT_EQ(nullptr, controller.getTextureByName("   "));
}

TEST_F(MapTextureControllerWith2Textures, getTextureByName_WithNotExistantName_ReturnNull)
{
    ASSERT_EQ(nullptr, controller.getTextureByName("tex3"));
}

TEST_F(MapTextureControllerWith2Textures, getTextureByName_WithTex1_ReturnTextureDTO)
{
    auto actual = controller.getTextureByName("tex1");
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("tex1", actual->name);
    ASSERT_EQ("tex1.png", actual->filename);
    ASSERT_EQ(256, actual->width);
    ASSERT_EQ(128, actual->height);
    ASSERT_EQ(32, actual->tileWidth);
    ASSERT_EQ(16, actual->tileHeight);
}

TEST_F(MapTextureControllerWith2Textures, addTexture_WithNull_ReturnFalse)
{
    ASSERT_FALSE(controller.addTexture(nullptr));
    ASSERT_EQ("textureDTO must be provided.", controller.getLastError());
}

TEST_F(MapTextureControllerWith2Textures, addTexture_WithNewValidTexture_ReturnTrue)
{
    auto newTexture = std::unique_ptr<TextureDTO>(new TextureDTO({
        "tex3", "tex3.png", 128, 256, 16, 32
    }));
    ASSERT_TRUE(controller.addTexture(std::move(newTexture)));
    ASSERT_EQ(3, controller.getTexturesNames().size());
    auto actual = controller.getTextureByName("tex3");
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("tex3", actual->name);
    ASSERT_EQ("tex3.png", actual->filename);
    ASSERT_EQ(128, actual->width);
    ASSERT_EQ(256, actual->height);
    ASSERT_EQ(16, actual->tileWidth);
    ASSERT_EQ(32, actual->tileHeight);
}

TEST_F(MapTextureControllerWith2Textures, replaceTexture_WithNull_ReturnFalse)
{
    ASSERT_FALSE(controller.replaceTexture("tex2", nullptr));
    ASSERT_EQ("textureDTO must be provided.", controller.getLastError());
}

TEST_F(MapTextureControllerWith2Textures, replaceTexture_WithNewValidTexture_ReturnTrue)
{
    auto newTexture = std::unique_ptr<TextureDTO>(new TextureDTO({
        "tex3", "tex3.png", 128, 256, 16, 32
    }));
    ASSERT_TRUE(controller.replaceTexture("tex2", std::move(newTexture)));
    ASSERT_EQ(2, controller.getTexturesNames().size());
    auto actual = controller.getTextureByName("tex3");
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("tex3", actual->name);
    ASSERT_EQ("tex3.png", actual->filename);
    ASSERT_EQ(128, actual->width);
    ASSERT_EQ(256, actual->height);
    ASSERT_EQ(16, actual->tileWidth);
    ASSERT_EQ(32, actual->tileHeight);
}
