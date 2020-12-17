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
