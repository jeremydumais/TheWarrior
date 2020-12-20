#include "texture.hpp"
#include <gtest/gtest.h>

using namespace std;

class TextureWithDefaultValidTextureInfo : public ::testing::Test
{
public:
	TextureWithDefaultValidTextureInfo()
	  : textureInfo {
			"file", "file.png",
			512, 256,
			32, 16
		}
	{}
	TextureInfo textureInfo;
};


TEST_F(TextureWithDefaultValidTextureInfo, Constructor_EmptyName_ThrowInvalidArgument)
{
	try
	{
		textureInfo.name = "";
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_WhiteSpacesName_ThrowInvalidArgument)
{
	try
	{
		textureInfo.name = "  ";
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_EmptyFilename_ThrowInvalidArgument)
{
	try
	{
		textureInfo.filename = "";
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_WhiteSpacesFilename_ThrowInvalidArgument)
{
	try
	{
		textureInfo.filename = "   ";
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_ZeroWidth_ThrowInvalidArgument)
{
	try
	{
		textureInfo.width = 0;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_Minus1Width_ThrowInvalidArgument)
{
	try
	{
		textureInfo.width = -1;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_ZeroHeight_ThrowInvalidArgument)
{
	try
	{
		textureInfo.height = 0;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_Minus1Height_ThrowInvalidArgument)
{
	try
	{
		textureInfo.height = -1;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_ZeroTileWidth_ThrowInvalidArgument)
{
	try
	{
		textureInfo.tileWidth = 0;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_Minus1TileWidth_ThrowInvalidArgument)
{
	try
	{
		textureInfo.tileWidth = -1;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_ZeroTileHeight_ThrowInvalidArgument)
{
	try
	{
		textureInfo.tileHeight = 0;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_Minus1TileHeight_ThrowInvalidArgument)
{
	try
	{
		textureInfo.tileHeight = -1;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_ValidInfo_ReturnSuccess)
{
	Texture texture(textureInfo);
	ASSERT_EQ("file", texture.getName());
	ASSERT_EQ("file.png", texture.getFilename());
	ASSERT_EQ(512, texture.getWidth());
	ASSERT_EQ(256, texture.getHeight());
	ASSERT_EQ(32, texture.getTileWidth());
	ASSERT_EQ(16, texture.getTileHeight());
}

TEST_F(TextureWithDefaultValidTextureInfo, getName_ValidName_ReturnValidName)
{
	Texture texture(textureInfo);
	ASSERT_EQ("file", texture.getName());
}

TEST_F(TextureWithDefaultValidTextureInfo, getFilename_ValidName_ReturnValidFilename)
{
	Texture texture(textureInfo);
	ASSERT_EQ("file.png", texture.getFilename());
}

TEST_F(TextureWithDefaultValidTextureInfo, getWidth_With512_Return512)
{
	Texture texture(textureInfo);
	ASSERT_EQ(512, texture.getWidth());
}

TEST_F(TextureWithDefaultValidTextureInfo, getHeight_With256_Return256)
{
	Texture texture(textureInfo);
	ASSERT_EQ(256, texture.getHeight());
}

TEST_F(TextureWithDefaultValidTextureInfo, getTileWidth_With32_Return32)
{
	Texture texture(textureInfo);
	ASSERT_EQ(32, texture.getTileWidth());
}

TEST_F(TextureWithDefaultValidTextureInfo, getTileHeight_With16_Return16)
{
	Texture texture(textureInfo);
	ASSERT_EQ(16, texture.getTileHeight());
}

TEST_F(TextureWithDefaultValidTextureInfo, setName_ValidName_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setName("file2");
	ASSERT_EQ("file2", texture.getName());
}

TEST_F(TextureWithDefaultValidTextureInfo, setName_EmptyName_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setName("");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setName_WhiteSpacesName_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setName("   ");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setFilename_ValidFilename_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setFilename("file2.png");
	ASSERT_EQ("file2.png", texture.getFilename());
}

TEST_F(TextureWithDefaultValidTextureInfo, setFilename_EmptyFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setFilename("");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setFilename_WhiteSpacesFilename_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setFilename("   ");
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("filename cannot be null or empty.", err.what());
	}
}