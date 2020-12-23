#include "mapTile.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(MapTile_Constructor, DefaultConstructor_ReturnEmptyTile)
{
	MapTile tile;
	ASSERT_EQ("", tile.getTextureName());
	ASSERT_EQ(-1, tile.getTextureIndex());
	ASSERT_EQ("", tile.getObjectTextureName());
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}

TEST(MapTile_getTextureName, DefaultConstructor_ReturnEmptyString)
{
	MapTile tile;
	ASSERT_EQ("", tile.getTextureName());
}

TEST(MapTile_getTextureName, withNameABC_ReturnABC)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
}

TEST(MapTile_getTextureIndex, DefaultConstructor_ReturnMinus1)
{
	MapTile tile;
	ASSERT_EQ(-1, tile.getTextureIndex());
}

TEST(MapTile_getTextureIndex, With1_Return1)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
}

TEST(MapTile_getObjectTextureName, DefaultConstructor_ReturnEmptyString)
{
	MapTile tile;
	ASSERT_EQ("", tile.getObjectTextureName());
}

TEST(MapTile_getObjectTextureName, withNameABC_ReturnABC)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
}

TEST(MapTile_getObjectTextureIndex, DefaultConstructor_ReturnMinus1)
{
	MapTile tile;
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}

TEST(MapTile_getObjectTextureIndex, With1_Return1)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
}

TEST(MapTile_setTextureName, WithABC_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
}

TEST(MapTile_setTextureName, WithEmptyName_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
	tile.setTextureName("");
	ASSERT_EQ("", tile.getTextureName());
}

TEST(MapTile_setTextureIndex, With1_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
}

TEST(MapTile_setTextureIndex, WithMinus1_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureIndex(1);
	ASSERT_EQ(1, tile.getTextureIndex());
	tile.setTextureIndex(-1);
	ASSERT_EQ(-1, tile.getTextureIndex());
}

TEST(MapTile_setObjectTextureName, WithABC_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
}

TEST(MapTile_setObjectTextureName, WithEmptyName_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureName("ABC");
	ASSERT_EQ("ABC", tile.getObjectTextureName());
	tile.setObjectTextureName("");
	ASSERT_EQ("", tile.getObjectTextureName());
}

TEST(MapTile_setObjectTextureIndex, With1_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
}

TEST(MapTile_setObjectTextureIndex, WithMinus1_ReturnSuccess)
{
	MapTile tile;
	tile.setObjectTextureIndex(1);
	ASSERT_EQ(1, tile.getObjectTextureIndex());
	tile.setObjectTextureIndex(-1);
	ASSERT_EQ(-1, tile.getObjectTextureIndex());
}
