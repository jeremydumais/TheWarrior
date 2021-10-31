#include "textureContainer.hpp"
#include <gtest/gtest.h>

using namespace std;

struct DefaultConstructedTextureContainer : public ::testing::Test
{
TextureContainer container;
};

struct TextureContainerWith2Textures : public ::testing::Test
{
TextureContainerWith2Textures() {
    container.addTexture({
				"a", "a.png",
				512, 512,
				32, 32
			});
	container.addTexture({
				"b", "b.png",
				512, 512,
				32, 32
			});
}
TextureContainer container;
};

TEST_F(DefaultConstructedTextureContainer, getTextures_EmptyTextureList_ReturnEmptyList)
{
	ASSERT_EQ(0, container.getTextures().size());
}

TEST_F(DefaultConstructedTextureContainer, getTextures_OneTexture_ReturnOneTexture)
{
	container.addTexture({
        "a", "a.png",
        512, 512,
        32, 32
	});
	ASSERT_EQ(1, container.getTextures().size());
	const auto &firstTexture = container.getTextures()[0];
	ASSERT_EQ("a", firstTexture.getName());
	ASSERT_EQ("a.png", firstTexture.getFilename());
}

TEST_F(TextureContainerWith2Textures, getTextures_TwoTextures_ReturnTwoTextures)
{
	const auto &firstTexture = container.getTextures()[0];
	ASSERT_EQ("a", firstTexture.getName());
	ASSERT_EQ("a.png", firstTexture.getFilename());
	const auto &secondTexture = container.getTextures()[1];
	ASSERT_EQ("b", secondTexture.getName());
	ASSERT_EQ("b.png", secondTexture.getFilename());
}

TEST_F(DefaultConstructedTextureContainer, ReturnSuccess)
{
    ASSERT_EQ(0, container.getCount());
}

TEST_F(DefaultConstructedTextureContainer, getTextureByName_EmptyName_ReturnEmpty)
{
	auto actual { container.getTextureByName("") };
	ASSERT_FALSE(actual.has_value());
}

TEST_F(DefaultConstructedTextureContainer, getTextureByName_WhiteSpacesName_ReturnEmpty)
{
	auto actual { container.getTextureByName("   ") };
	ASSERT_FALSE(actual.has_value());
}

TEST_F(DefaultConstructedTextureContainer, getTextureByName_NonExistantName_ReturnEmpty)
{
	auto actual { container.getTextureByName("s") };
	ASSERT_FALSE(actual.has_value());
}

TEST_F(TextureContainerWith2Textures, getTextureByName_ExistingNameWithListOfTwoTextures_ReturnTexture)
{
	auto actual { container.getTextureByName("b") };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ("b", actual->getName());
	ASSERT_EQ("b.png", actual->getFilename());
}

TEST_F(DefaultConstructedTextureContainer, addTexture_FirstTextureUniqueName_ReturnTrue)
{
	TextureInfo textureInfo {
			"Terrain", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(container.addTexture(textureInfo));	
}

TEST_F(DefaultConstructedTextureContainer, addTexture_SameTextureNameTwice_ReturnFalse)
{
	TextureInfo textureInfo {
			"Terrain", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(container.addTexture(textureInfo));	
	ASSERT_FALSE(container.addTexture(textureInfo));	
}

TEST_F(DefaultConstructedTextureContainer, addTexture_EmptyName_ReturnFalse)
{
	TextureInfo textureInfo {
			"", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(container.addTexture(textureInfo));	
	ASSERT_EQ("name cannot be null or empty.", container.getLastError());
}

TEST_F(TextureContainerWith2Textures, replaceTexture_WithNonExistantName_ReturnFalse)
{
	TextureInfo textureInfo {
			"file", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(container.replaceTexture("tex99", textureInfo));
	ASSERT_EQ("Unable to find the texture tex99 in the texture list.", container.getLastError());
}

TEST_F(TextureContainerWith2Textures, replaceTexture_WithInvalidNewTextureInfo_ReturnFalse)
{
	TextureInfo textureInfo {
			"", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(container.replaceTexture("a", textureInfo));
	ASSERT_EQ("name cannot be null or empty.", container.getLastError());
}

TEST_F(TextureContainerWith2Textures, replaceTexture_WithAlreadyExistingNewName_ReturnFalse)
{
	TextureInfo textureInfo {
			"b", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(container.replaceTexture("a", textureInfo));
	ASSERT_EQ("The texture b already exist in the texture list.", container.getLastError());
}

TEST_F(TextureContainerWith2Textures, replaceTexture_WithValidArgs_ReturnTrue)
{
	TextureInfo textureInfo {
			"file", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(container.replaceTexture("a", textureInfo));
}

TEST_F(TextureContainerWith2Textures, removeTexture_WithInexisantName_ReturnFalse)
{
	ASSERT_FALSE(container.removeTexture("tex99"));
	ASSERT_EQ("Unable to find the texture tex99 in the texture list.", container.getLastError());
}

TEST_F(TextureContainerWith2Textures, removeTexture_WithValidName_ReturnTrue)
{
	ASSERT_TRUE(container.removeTexture("b"));
}