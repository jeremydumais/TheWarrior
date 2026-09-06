#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include "iTexturePixmapProvider.hpp"
#include "selectNPCTextureFormController.hpp"
#include "texture.hpp"
#include "textureInfo.hpp"

using commoneditor::ui::ITexturePixmapProvider;
using thewarrior::models::NPCSpriteLayout;
using thewarrior::models::Texture;
using thewarrior::models::TextureInfo;

namespace mapeditor::controllers::selectnpctextureformcontroller::unittest {

class FakeTexturePixmapProvider final : public ITexturePixmapProvider {
 public:
    std::shared_ptr<QPixmap> loadPixmap(const std::string &) override {
        return nullptr;
    }
    ~FakeTexturePixmapProvider() override;
};

FakeTexturePixmapProvider::~FakeTexturePixmapProvider() {}

class SelectNPCTextureFormControllerWith3Textures : public ::testing::Test {
 public:
    SelectNPCTextureFormControllerWith3Textures()
    : textures({
        Texture(TextureInfo {
                "file", "file.png",
                544, 256,
                32, 16
                }),
        Texture(TextureInfo {
                "file2", "file2.png",
                96, 128,
                32, 32
                }),
        Texture(TextureInfo {
                "file3", "file3.png",
                288, 384,
                32, 32
                }),
        }),
    controller("", textures, texturePixmapProvider, NPCSpriteLayout::Direction12Frames) {}
    ~SelectNPCTextureFormControllerWith3Textures() override;
    std::vector<Texture> textures;
    FakeTexturePixmapProvider texturePixmapProvider;
    SelectNPCTextureFormController controller;
};

SelectNPCTextureFormControllerWith3Textures::~SelectNPCTextureFormControllerWith3Textures() {}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withNonExistingTexture_ReturnError) {
    const auto result = controller.getAvailableNPCs("notExist");
    ASSERT_FALSE(result.success);
    ASSERT_EQ("The texture notExist could not be found.", controller.getLastError());
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withNonNPCTexture_ReturnError) {
    const auto result = controller.getAvailableNPCs("file");
    ASSERT_FALSE(result.success);
    ASSERT_EQ("The selected texture does not match the required NPC format. NPC textures must be 3 tiles wide and 4 tiles high.", controller.getLastError());
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withNPCTextureFile2_ReturnOneNPC) {
    const auto result = controller.getAvailableNPCs("file2");
    ASSERT_TRUE(result.success);
    ASSERT_EQ(1, result.result.size());
    ASSERT_EQ(0, result.result.at(0).baseTextureIndex);
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withNPCTextureFile3_ReturnNineNPCs) {
    const auto result = controller.getAvailableNPCs("file3");
    ASSERT_TRUE(result.success);
    ASSERT_EQ(9, result.result.size());
    ASSERT_EQ(0, result.result.at(0).baseTextureIndex);
    ASSERT_EQ(3, result.result.at(1).baseTextureIndex);
    ASSERT_EQ(6, result.result.at(2).baseTextureIndex);
    ASSERT_EQ(36, result.result.at(3).baseTextureIndex);
    ASSERT_EQ(39, result.result.at(4).baseTextureIndex);
    ASSERT_EQ(42, result.result.at(5).baseTextureIndex);
    ASSERT_EQ(72, result.result.at(6).baseTextureIndex);
    ASSERT_EQ(75, result.result.at(7).baseTextureIndex);
    ASSERT_EQ(78, result.result.at(8).baseTextureIndex);
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withSingleFrameAndNonNPCTexture_ReturnEveryTile) {
    SelectNPCTextureFormController singleFrameController("", textures, texturePixmapProvider, NPCSpriteLayout::SingleFrame);

    const auto result = singleFrameController.getAvailableNPCs("file");

    ASSERT_TRUE(result.success);
    ASSERT_EQ(17 * 16, result.result.size());
    for (size_t index = 0; index < result.result.size(); ++index) {
        EXPECT_EQ(static_cast<int>(index), result.result[index].baseTextureIndex);
    }
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withSingleFrameAndNPCTexture_ReturnIndividualTiles) {
    SelectNPCTextureFormController singleFrameController("", textures, texturePixmapProvider, NPCSpriteLayout::SingleFrame);

    const auto result = singleFrameController.getAvailableNPCs("file2");

    ASSERT_TRUE(result.success);
    ASSERT_EQ(12, result.result.size());
    for (size_t index = 0; index < result.result.size(); ++index) {
        EXPECT_EQ(static_cast<int>(index), result.result[index].baseTextureIndex);
    }
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withSingleFrameAndMissingTexture_ReturnError) {
    SelectNPCTextureFormController singleFrameController("", textures, texturePixmapProvider, NPCSpriteLayout::SingleFrame);

    const auto result = singleFrameController.getAvailableNPCs("notExist");

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.result.empty());
    EXPECT_EQ("The texture notExist could not be found.", singleFrameController.getLastError());
}

TEST_F(SelectNPCTextureFormControllerWith3Textures, getAvailableNPCs_withSingleTileTexture_ReturnOneTile) {
    textures.emplace_back(TextureInfo {"single", "single.png", 32, 32, 32, 32});
    SelectNPCTextureFormController singleFrameController("", textures, texturePixmapProvider, NPCSpriteLayout::SingleFrame);

    const auto result = singleFrameController.getAvailableNPCs("single");

    ASSERT_TRUE(result.success);
    ASSERT_EQ(1, result.result.size());
    EXPECT_EQ(0, result.result.front().baseTextureIndex);
}

}  // namespace mapeditor::controllers::selectnpctextureformcontroller::unittest
