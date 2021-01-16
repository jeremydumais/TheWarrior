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

TEST(MapTile_isAssigned, WithAllFieldsEmpty_ReturnFalse)
{
	MapTile tile;
	ASSERT_FALSE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyTextureNameNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyTextureIndex0_ReturnTrue)
{
	MapTile tile;
	tile.setTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyObjectTextureNameNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setObjectTextureName("Tex1");
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithOnlyObjectTextureIndex0_ReturnTrue)
{
	MapTile tile;
	tile.setObjectTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_setTextureName, WithABC_ReturnSuccess)
{
	MapTile tile;
	tile.setTextureName("ABC");
	ASSERT_EQ("ABC", tile.getTextureName());
}

TEST(MapTile_isAssigned, WithTextureNameAndIndexNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	tile.setTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
}

TEST(MapTile_isAssigned, WithAllFieldsNotEmpty_ReturnTrue)
{
	MapTile tile;
	tile.setTextureName("Tex1");
	tile.setTextureIndex(0);
	tile.setObjectTextureName("Tex1");
	tile.setObjectTextureIndex(0);
	ASSERT_TRUE(tile.isAssigned());
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
