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


// N = Not assigned, A = Assigned
//      0 1 2 3 4 5
//      -----------
//   0  N N N N N N
//   6  N N A N N N
//  12  N A A A N N
//  18  N N A N N N
//  24  N N N N A N
//  30  N N N N N N
class SampleGameMapWithTilesAssigned : public ::testing::Test
{
public:
    SampleGameMapWithTilesAssigned()
		: map(6, 6) {
        map.addTexture({
            "tex1",
            "tex1.png",
            512, 256,
            32, 32
        });
        auto &tile8 { map.getTileForEditing(8) };
        tile8.setTextureName("tex1");
        tile8.setTextureIndex(0); 
        auto &tile13 { map.getTileForEditing(13) };
        tile13.setTextureIndex(0); 
        auto &tile14 { map.getTileForEditing(14) };
        tile14.setTextureIndex(0); 
        auto &tile15 { map.getTileForEditing(15) };
        tile15.setTextureIndex(1); 
        auto &tile20 { map.getTileForEditing(20) };
        tile20.setObjectTextureIndex(1); 
        auto &tile28 { map.getTileForEditing(28) };
        tile28.setObjectTextureName("tex1"); 
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

TEST(GameMap_getHeight, HeightFive_ReturnFive)
{
	GameMap map(6, 5);
	ASSERT_EQ(5, map.getHeight());
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


TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnLeft_ReturnFalse)
{
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(-1, 0, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnLeft_ReturnTrue)
{
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(-2, 0, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnTop_ReturnFalse)
{
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, -1, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnTop_ReturnTrue)
{
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, -2, 0, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnRight_ReturnFalse)
{
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, 0, -1, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnRight_ReturnTrue)
{
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, 0, -2, 0));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnBottom_ReturnFalse)
{
    ASSERT_FALSE(map.isShrinkMapImpactAssignedTiles(0, 0, 0, -1));
}

TEST_F(SampleGameMapWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnBottom_ReturnTrue)
{
    ASSERT_TRUE(map.isShrinkMapImpactAssignedTiles(0, 0, 0, -2));
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnLeft_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the first column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[0].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnLeft_ReturnSuccess)
{
    map.resizeMap(-1, 0, 0, 0);
    ASSERT_EQ(5, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new first column
	ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[1][0].isAssigned());
	ASSERT_TRUE(map.getTiles()[2][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[3][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[4][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[5][0].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnLeft_ReturnSuccess)
{
    map.resizeMap(-2, 0, 0, 0);
    ASSERT_EQ(4, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new first column
	ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
	ASSERT_TRUE(map.getTiles()[1][0].isAssigned());
	ASSERT_TRUE(map.getTiles()[2][0].isAssigned());
	ASSERT_TRUE(map.getTiles()[3][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[4][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[5][0].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnLeft_ReturnSuccess)
{
    map.resizeMap(1, 0, 0, 0);
    ASSERT_EQ(7, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new first column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[0].isAssigned());
		ASSERT_FALSE(row[1].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusTwoOnLeft_ReturnSuccess)
{
    map.resizeMap(2, 0, 0, 0);
    ASSERT_EQ(8, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new first column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[0].isAssigned());
		ASSERT_FALSE(row[1].isAssigned());
		ASSERT_FALSE(row[2].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnTop_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the first row
	for(int i=0; i<map.getWidth(); i++) {
		ASSERT_FALSE(map.getTiles()[0][i].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnTop_ReturnSuccess)
{
    map.resizeMap(0, -1, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(5, map.getHeight());
	//Check the new first row
	ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][1].isAssigned());
	ASSERT_TRUE(map.getTiles()[0][2].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][3].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][4].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnTop_ReturnSuccess)
{
    map.resizeMap(0, -2, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(4, map.getHeight());
	//Check the new first row
	ASSERT_FALSE(map.getTiles()[0][0].isAssigned());
	ASSERT_TRUE(map.getTiles()[0][1].isAssigned());
	ASSERT_TRUE(map.getTiles()[0][2].isAssigned());
	ASSERT_TRUE(map.getTiles()[0][3].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][4].isAssigned());
	ASSERT_FALSE(map.getTiles()[0][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnTop_ReturnSuccess)
{
    map.resizeMap(0, 1, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(7, map.getHeight());
	//Check the new first row
	for(int i=0; i<map.getWidth(); i++) {
		ASSERT_FALSE(map.getTiles()[0][i].isAssigned());
		ASSERT_FALSE(map.getTiles()[1][i].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnRight_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the last column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnRight_ReturnSuccess)
{
    map.resizeMap(0, 0, -1, 0);
    ASSERT_EQ(5, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	size_t lastCol { map.getWidth()-1 };
	//Check the new last column
	ASSERT_FALSE(map.getTiles()[0][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[1][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[2][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[3][lastCol].isAssigned());
	ASSERT_TRUE(map.getTiles()[4][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[5][lastCol].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnRight_ReturnSuccess)
{
    map.resizeMap(0, 0, -2, 0);
    ASSERT_EQ(4, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	size_t lastCol { map.getWidth()-1 };
	//Check the new last column
	ASSERT_FALSE(map.getTiles()[0][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[1][lastCol].isAssigned());
	ASSERT_TRUE(map.getTiles()[2][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[3][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[4][lastCol].isAssigned());
	ASSERT_FALSE(map.getTiles()[5][lastCol].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnRight_ReturnSuccess)
{
    map.resizeMap(0, 0, 1, 0);
    ASSERT_EQ(7, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new last column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
		ASSERT_FALSE(row[map.getWidth()-2].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusTwoOnRight_ReturnSuccess)
{
    map.resizeMap(0, 0, 2, 0);
    ASSERT_EQ(8, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the new last column
	for(const auto &row : map.getTiles()) {
		ASSERT_FALSE(row[map.getWidth()-1].isAssigned());
		ASSERT_FALSE(row[map.getWidth()-2].isAssigned());
		ASSERT_FALSE(row[map.getWidth()-3].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithZeroOnBottom_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, 0);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(6, map.getHeight());
	//Check the last row
	for(int i=0; i<map.getWidth(); i++) {
		ASSERT_FALSE(map.getTiles()[map.getHeight() - 1][i].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusOneOnBottom_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, -1);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(5, map.getHeight());
	size_t lastRow { map.getHeight() - 1 };
	//Check the new last row
	ASSERT_FALSE(map.getTiles()[lastRow][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][1].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][2].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][3].isAssigned());
	ASSERT_TRUE(map.getTiles()[lastRow][4].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithMinusTwoOnBottom_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, -2);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(4, map.getHeight());
	size_t lastRow { map.getHeight() - 1 };
	//Check the new last row
	ASSERT_FALSE(map.getTiles()[lastRow][0].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][1].isAssigned());
	ASSERT_TRUE(map.getTiles()[lastRow][2].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][3].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][4].isAssigned());
	ASSERT_FALSE(map.getTiles()[lastRow][5].isAssigned());
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithPlusOneOnBottom_ReturnSuccess)
{
    map.resizeMap(0, 0, 0, 1);
    ASSERT_EQ(6, map.getWidth());
    ASSERT_EQ(7, map.getHeight());
	//Check the new last row
	for(int i=0; i<map.getWidth(); i++) {
		ASSERT_FALSE(map.getTiles()[map.getHeight() - 1][i].isAssigned());
		ASSERT_FALSE(map.getTiles()[map.getHeight() - 2][i].isAssigned());
	}
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetLeftNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument)
{
	try
	{
		map.resizeMap(-6, 0, 0, 0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("This left offset would remove all the remaining tiles.", err.what());
	}	
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetTopNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument)
{
	try
	{
		map.resizeMap(0, -6, 0, 0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("This top offset would remove all the remaining tiles.", err.what());
	}	
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetRightNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument)
{
	try
	{
		map.resizeMap(0, 0, -6, 0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("This right offset would remove all the remaining tiles.", err.what());
	}	
}

TEST_F(SampleGameMapWithTilesAssigned, resizeMap_WithOffsetBottomNegativeAndGreaterThanRemainingTiles_ThrowInvalidArgument)
{
	try
	{
		map.resizeMap(0, 0, 0, -6);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("This bottom offset would remove all the remaining tiles.", err.what());
	}	
}
