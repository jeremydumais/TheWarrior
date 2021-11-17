#include "manageTexturesController.hpp"
#include <gtest/gtest.h>

using namespace std;

class MapTextureControllerWith2Textures : public ::testing::Test
{
public:
	MapTextureControllerWith2Textures()
        : controller(textureContainer)
	{
		textureContainer.addTexture({
            "tex1", "tex1.png",
            256, 256,
            32, 32
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
    auto actual = controller.getTextures();
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("tex1", actual.at(0).getName());
    ASSERT_EQ("tex2", actual.at(1).getName());
}

TEST_F(MapTextureControllerWith2Textures, getAlreadyUsedNames_Return2Names)
{
    auto actual = controller.getAlreadyUsedNames();
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("tex1", actual.at(0));
    ASSERT_EQ("tex2", actual.at(1));
}
