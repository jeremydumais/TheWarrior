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
    controller("", textures, texturePixmapProvider) {}
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

}  // namespace mapeditor::controllers::selectnpctextureformcontroller::unittest
