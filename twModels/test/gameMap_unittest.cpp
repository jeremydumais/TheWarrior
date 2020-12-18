#include "gameMap.hpp"
#include <gtest/gtest.h>

using namespace std;

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
	map.addTexture("a", "a.png");
	ASSERT_EQ(1, map.getTextures().size());
	const auto &firstTexture = map.getTextures()[0];
	ASSERT_EQ("a", firstTexture.getName());
	ASSERT_EQ("a.png", firstTexture.getFilename());
}

TEST(GameMap_getTextures, TwoTextures_ReturnTwoTextures)
{
	GameMap map(5, 6);
	map.addTexture("a", "a.png");
	map.addTexture("b", "b.png");
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
	map.addTexture("a", "a.png");
	auto actual { map.getTextureByName("a") };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ("a", actual->getName());
	ASSERT_EQ("a.png", actual->getFilename());
}

TEST(GameMap_getTextureByName, ExistingNameWithListOfTwoTextures_ReturnTexture)
{
	GameMap map(5, 6);
	map.addTexture("a", "a.png");
	map.addTexture("b", "b.png");
	auto actual { map.getTextureByName("b") };
	ASSERT_TRUE(actual.has_value());
	ASSERT_EQ("b", actual->getName());
	ASSERT_EQ("b.png", actual->getFilename());
}

TEST(GameMap_addTexture, FirstTextureUniqueName_ReturnTrue)
{
	GameMap map(5, 6);
	ASSERT_TRUE(map.addTexture("Terrain", "file.png"));	
}

TEST(GameMap_addTexture, SameTextureNameTwice_ReturnFalse)
{
	GameMap map(5, 6);
	ASSERT_TRUE(map.addTexture("Terrain", "file.png"));	
	ASSERT_FALSE(map.addTexture("Terrain", "file.png"));	
}