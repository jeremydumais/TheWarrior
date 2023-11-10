#include <gtest/gtest.h>
#include "point.hpp"
#include "texture.hpp"
#include "textureDTO.hpp"
#include "textureInfo.hpp"
#include "textureUtils.hpp"

using commoneditor::ui::TextureUtils;
using commoneditor::ui::TextureDTO;
using thewarrior::models::Texture;
using thewarrior::models::TextureInfo;
using thewarrior::models::Point;

Texture getTextureSample1() {
    return Texture({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
    });
}

void TextureInfoComparer(TextureInfo val1, TextureInfo val2) {
    ASSERT_EQ(val1.name, val2.name);
    ASSERT_EQ(val1.filename, val2.filename);
    ASSERT_EQ(val1.width, val2.width);
    ASSERT_EQ(val1.height, val2.height);
    ASSERT_EQ(val1.tileWidth, val2.tileWidth);
    ASSERT_EQ(val1.tileHeight, val2.tileHeight);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_255_Return0) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 255),
                                                            getTextureSample1());
    ASSERT_EQ(0, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample32_255_Return1) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(32, 255),
                                                            getTextureSample1());
    ASSERT_EQ(1, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample2_223_Return16) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(2, 223),
                                                            getTextureSample1());
    ASSERT_EQ(16, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_224_Return16) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 224),
                                                            getTextureSample1());
    ASSERT_EQ(16, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_127AndRatio0_5F_Return0) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 127),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(0, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample31_128AndRatio0_5F_Return1) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(31, 128),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(1, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample32_128AndRatio0_5F_Return1) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(32, 128),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(2, actual);
}

TEST(TextureUtils_getTextureIndexFromPosition, WithSample0_112AndRatio0_5F_Return16) {
    auto actual = TextureUtils::getTextureIndexFromPosition(Point(0, 112),
                                                            getTextureSample1(),
                                                            0.5F);
    ASSERT_EQ(16, actual);
}

TEST(TextureUtils_textureDTOToTextureInfo, WithEmptyDTO_ReturnEmptyInfo) {
    TextureDTO dto;
    TextureInfo expected;
    auto actual = TextureUtils::TextureDTOToTextureInfo(dto);
    TextureInfoComparer(expected, actual);
}

TEST(TextureUtils_textureDTOToTextureInfo, WithNonEmptyDTO_ReturnNonEmptyInfo) {
    TextureDTO dto { "Test", "Test1", 1, 2, 3, 4 };
    TextureInfo expected { "Test", "Test1", 1, 2, 3, 4 };
    auto actual = TextureUtils::TextureDTOToTextureInfo(dto);
    TextureInfoComparer(expected, actual);
}

