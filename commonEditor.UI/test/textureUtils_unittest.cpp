#include "textureUtils.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace commoneditor::ui;

Texture getTextureSample1() {
    return Texture({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
    });
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_255_Return0)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 255),
                                                            getTextureSample1());
    ASSERT_EQ(0, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample32_255_Return1)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(32, 255),
                                                            getTextureSample1());
    ASSERT_EQ(1, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample2_223_Return16)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(2, 223),
                                                            getTextureSample1());
    ASSERT_EQ(16, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_224_Return16)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 224),
                                                            getTextureSample1());
    ASSERT_EQ(16, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_127AndRatio0_5F_Return0)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 127),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(0, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample31_128AndRatio0_5F_Return1)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(31, 128),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(1, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample32_128AndRatio0_5F_Return1)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(32, 128),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(2, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_112AndRatio0_5F_Return16)
{
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 112),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(16, actual);
}

