#include "textureSelectionComponentController.hpp"
#include "texture.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(TextureSelectionComponentController_getTextureIndexFromPosition, WithSample1_ReturnValidPosition)
{
    TextureSelectionComponentController controller;
    Texture tex1({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
    auto actual = controller.getTextureIndexFromPosition(Point(0, 255), tex1);
    ASSERT_EQ(0, actual);
}

TEST(TextureSelectionComponentController_getTextureIndexFromPosition, WithSample2_ReturnValidPosition)
{
    TextureSelectionComponentController controller;
    Texture tex1({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
    auto actual = controller.getTextureIndexFromPosition(Point(32, 255), tex1);
    ASSERT_EQ(1, actual);
}