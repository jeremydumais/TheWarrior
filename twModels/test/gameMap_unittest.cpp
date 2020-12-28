#include "gameMap.hpp"
#include <gtest/gtest.h>

using namespace std;

class SampleGameMap5x6WithTwoTextures : public ::testing::Test
{
public:
	SampleGameMap5x6WithTwoTextures()
	  : map(5, 6)
	{
		map.addTexture({
				"a", "a.png",
				512, 512,
				32, 32
			});

		map.addTexture({
				"b", "b.png",
				512, 512,
				32, 32
			});
		const auto &firstTexture = map.getTextures()[0];
		auto &tileIndex0 = map.getTileForEditing(0);
		tileIndex0.setTextureName(firstTexture.getName());
		tileIndex0.setTextureIndex(0);
		tileIndex0.setObjectTextureName(firstTexture.getName());
		tileIndex0.setObjectTextureIndex(0);
	}
	GameMap map;
};

TEST(GameMap_Constructor, ZeroWidth_ThrowInvalidArgument)
{
	try
	{
		GameMap map(0, 10);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("width must be greater than zero.", err.what());
	}
}

TEST(GameMap_Constructor, ZeroHeight_ThrowInvalidArgument)
{
	try
	{
		GameMap map(10, 0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("height must be greater than zero.", err.what());
	}
}

TEST(GameMap_getWidth, WidthFive_ReturnFive)
{
	GameMap map(5, 10);
	ASSERT_EQ(5, map.getWidth());
}

TEST(GameMap_getHeight, WidthSix_ReturnSix)
{
	GameMap map(5, 6);
	ASSERT_EQ(6, map.getHeight());
}

TEST(GameMap_getTextures, EmptyTextureList_ReturnEmptyList)
{
	GameMap map(5, 6);
	ASSERT_EQ(0, map.getTextures().size());
}

TEST(GameMap_getTextures, OneTexture_ReturnOneTexture)
{
	GameMap map(5, 6);
	TextureInfo textureInfo {
			"a", "a.png",
			512, 512,
			32, 32
		};
	map.addTexture(textureInfo);
	ASSERT_EQ(1, map.getTextures().size());
	const auto &firstTexture = map.getTextures()[0];
	ASSERT_EQ("a", firstTexture.getName());
	ASSERT_EQ("a.png", firstTexture.getFilename());
}

TEST(GameMap_getTextures, TwoTextures_ReturnTwoTextures)
{
	GameMap map(5, 6);
	TextureInfo textureInfoA {
			"a", "a.png",
			512, 512,
			32, 32
		};
	map.addTexture(textureInfoA);
	TextureInfo textureInfoB {
			"b", "b.png",
			512, 512,
			32, 32
		};
	map.addTexture(textureInfoB);
	ASSERT_EQ(2, map.getTextures().size());
	const auto &firstTexture = map.getTextures()[0];
	ASSERT_EQ("a", firstTexture.getName());
	ASSERT_EQ("a.png", firstTexture.getFilename());
	const auto &secondTexture = map.getTextures()[1];
	ASSERT_EQ("b", secondTexture.getName());
	ASSERT_EQ("b.png", secondTexture.getFilename());
}

TEST(GameMap_getTextureByName, EmptyName_ReturnEmpty)
{
	GameMap map(5, 6);
	auto actual { map.getTextureByName("") };
	ASSERT_FALSE(actual.has_value());
}

TEST(GameMap_getTextureByName, WhiteSpacesName_ReturnEmpty)
{
	GameMap map(5, 6);
	auto actual { map.getTextureByName("   ") };
	ASSERT_FALSE(actual.has_value());
}

TEST(GameMap_getTextureByName, NonExistantName_ReturnEmpty)
{
	GameMap map(5, 6);
	auto actual { map.getTextureByName("s") };
	ASSERT_FALSE(actual.has_value());
}

TEST(GameMap_getTextureByName, ExistingNameWithListOfOneTexture_ReturnTexture)
{
	GameMap map(5, 6);
	TextureInfo textureInfoA {
			"a", "a.png",
			512, 512,
			32, 32
		};
	map.addTexture(textureInfoA);
	auto actual { map.getTextureByName("a") };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ("a", actual->getName());
	ASSERT_EQ("a.png", actual->getFilename());
}

TEST_F(SampleGameMap5x6WithTwoTextures, getTextureByName_ExistingNameWithListOfTwoTextures_ReturnTexture)
{
	auto actual { map.getTextureByName("b") };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ("b", actual->getName());
	ASSERT_EQ("b.png", actual->getFilename());
}

TEST(GameMap_addTexture, FirstTextureUniqueName_ReturnTrue)
{
	GameMap map(5, 6);
	TextureInfo textureInfo {
			"Terrain", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(map.addTexture(textureInfo));	
}

TEST(GameMap_addTexture, SameTextureNameTwice_ReturnFalse)
{
	GameMap map(5, 6);
	TextureInfo textureInfo {
			"Terrain", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(map.addTexture(textureInfo));	
	ASSERT_FALSE(map.addTexture(textureInfo));	
}

TEST(GameMap_addTexture, EmptyName_ReturnFalse)
{
	GameMap map(5, 6);
	TextureInfo textureInfo {
			"", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(map.addTexture(textureInfo));	
	ASSERT_EQ("name cannot be null or empty.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceTexture_WithNonExistantName_ReturnFalse)
{
	TextureInfo textureInfo {
			"file", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(map.replaceTexture("tex99", textureInfo));
	ASSERT_EQ("Unable to find the texture tex99 in the texture list.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceTexture_WithInvalidNewTextureInfo_ReturnFalse)
{
	TextureInfo textureInfo {
			"", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(map.replaceTexture("a", textureInfo));
	ASSERT_EQ("name cannot be null or empty.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceTexture_WithAlreadyExistingNewName_ReturnFalse)
{
	TextureInfo textureInfo {
			"b", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_FALSE(map.replaceTexture("a", textureInfo));
	ASSERT_EQ("The texture b already exist in the texture list.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, replaceTexture_WithValidArgs_ReturnTrue)
{
	TextureInfo textureInfo {
			"file", "file.png",
			512, 512,
			32, 32
		};
	ASSERT_TRUE(map.replaceTexture("a", textureInfo));
}

TEST_F(SampleGameMap5x6WithTwoTextures, removeTexture_WithInexisantName_ReturnFalse)
{
	ASSERT_FALSE(map.removeTexture("tex99"));
	ASSERT_EQ("Unable to find the texture tex99 in the texture list.", map.getLastError());
}

TEST_F(SampleGameMap5x6WithTwoTextures, removeTexture_WithValidName_ReturnTrue)
{
	ASSERT_TRUE(map.removeTexture("b"));
}