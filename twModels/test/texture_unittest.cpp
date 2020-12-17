#include "texture.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(Texture_Constructor, EmptyName_ThrowInvalidArgument)
{
	try
	{
		Texture texture("", "file.png");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Texture_Constructor, WhiteSpacesName_ThrowInvalidArgument)
{
	try
	{
		Texture texture("  ", "file.png");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Texture_Constructor, EmptyFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST(Texture_Constructor, WhiteSpacesFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "    ");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST(Texture_GetName, ValidName_ReturnValidName)
{
	Texture texture("file", "file.png");
	ASSERT_EQ("file", texture.getName());
}

TEST(Texture_GetFilename, ValidName_ReturnValidFilename)
{
	Texture texture("file", "file.png");
	ASSERT_EQ("file.png", texture.getFilename());
}

TEST(TextureSetName, ValidName_ReturnSuccess)
{
	Texture texture("file", "file.png");
	ASSERT_EQ("file", texture.getName());
	texture.setName("file2");
	ASSERT_EQ("file2", texture.getName());
}

TEST(Texture_setName, EmptyName_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "file.png");
		ASSERT_EQ("file", texture.getName());
		texture.setName("");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Texture_setName, WhiteSpacesName_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "file.png");
		ASSERT_EQ("file", texture.getName());
		texture.setName("   ");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(TextureSetFilename, ValidFilename_ReturnSuccess)
{
	Texture texture("file", "file.png");
	ASSERT_EQ("file.png", texture.getFilename());
	texture.setFilename("file2.png");
	ASSERT_EQ("file2.png", texture.getFilename());
}

TEST(Texture_setFilename, EmptyFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "file.png");
		ASSERT_EQ("file.png", texture.getFilename());
		texture.setFilename("");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST(Texture_setFilename, WhiteSpacesFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture("file", "file.png");
		ASSERT_EQ("file.png", texture.getFilename());
		texture.setFilename("   ");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}
