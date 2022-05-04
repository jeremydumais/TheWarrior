#include "texturePickerController.hpp"
#include <gtest/gtest.h>

class TextureContainerSample1 : public ::testing::Test
{
public:
	TextureContainerSample1()
        : controller(textureContainer)
	{
        textureContainer.addTexture(TextureInfo { 
			"file", "file.png",
			512, 256,
			32, 16
		});
        textureContainer.addTexture(TextureInfo { 
			"file2", "file2.png",
			128, 512,
			8, 32
		});
    }
    TexturePickerController controller;
private:
    TextureContainer textureContainer;
};

TEST_F(TextureContainerSample1, getTextureNames_return2Names) 
{
    auto actual = controller.getTextureNames();
    ASSERT_EQ(2, actual.size());
    ASSERT_STREQ("file", actual[0].c_str());
    ASSERT_STREQ("file2", actual[1].c_str());
}

TEST_F(TextureContainerSample1, IsTextureExist_WithTest_ReturnFalse) 
{
    ASSERT_FALSE(controller.isTextureExist("Test"));
}

TEST_F(TextureContainerSample1, IsTextureExist_WithFile_ReturnTrue) 
{
    ASSERT_TRUE(controller.isTextureExist("file"));
}

TEST_F(TextureContainerSample1, IsTextureExist_WithFile2_ReturnTrue) 
{
    ASSERT_TRUE(controller.isTextureExist("file2"));
}

TEST_F(TextureContainerSample1, IsTextureExist_WithFileInCaps_ReturnFalse) 
{
    ASSERT_FALSE(controller.isTextureExist("FILE"));
}

TEST_F(TextureContainerSample1, getTextureFileName_WithTmpAndFile_ReturnValidPath) 
{
    ASSERT_EQ("/tmp/file.png", controller.getTextureFileName("/tmp", "file"));
}

TEST_F(TextureContainerSample1, getTextureFileName_WithEmptyResPathAndFile_ReturnValidPath) 
{
    ASSERT_EQ("file.png", controller.getTextureFileName("", "file"));
}

TEST_F(TextureContainerSample1, getTextureFileName_WithTmpAndFile2_ReturnValidPath) 
{
    ASSERT_EQ("/tmp/file2.png", controller.getTextureFileName("/tmp", "file2"));
}

TEST_F(TextureContainerSample1, getTextureTileImageFromTexture_With1_255AndFile_Return0) 
{
    ASSERT_EQ(0, controller.getTextureIndexFromPosition(Point(1, 255), "file"));
}

TEST_F(TextureContainerSample1, getTextureTileImageFromTexture_With1_255AndFile8_ReturnMinus1) 
{
    ASSERT_EQ(-1, controller.getTextureIndexFromPosition(Point(1, 255), "file8"));
}