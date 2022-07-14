#include "textureUtils.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(TextureUtils_getTextureIndexFromPosition, WithSample1_ReturnValidPosition)
{
    Texture tex1({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 255), tex1);
    ASSERT_EQ(0, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample2_ReturnValidPosition)
{
    Texture tex1({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(32, 255), tex1);
    ASSERT_EQ(1, actual);
}