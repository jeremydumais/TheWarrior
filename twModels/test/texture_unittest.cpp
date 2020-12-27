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

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_WidthLessThanTileWidth_ThrowInvalidArgument)
{
	try
	{
		textureInfo.width = 31;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be less than the width.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, Constructor_HeightLessThanTileHeight_ThrowInvalidArgument)
{
	try
	{
		textureInfo.height = 15;
		Texture texture(textureInfo);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be less than the height.", err.what());
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

TEST_F(TextureWithDefaultValidTextureInfo, getTileWidthGL_AssignedByConstructorWith512And32_Return0_0625)
{
	Texture texture(textureInfo);
	ASSERT_EQ(0.0625f, texture.getTileWidthGL());	
}

TEST_F(TextureWithDefaultValidTextureInfo, getTileWidthGL_AssignedByConstructorWith32And32_Return1)
{
	textureInfo.width = 32;
	Texture texture(textureInfo);
	ASSERT_EQ(1.0f, texture.getTileWidthGL());	
}

TEST_F(TextureWithDefaultValidTextureInfo, getTileHeightGL_AssignedByConstructorWith256And16_Return0_0625)
{
	Texture texture(textureInfo);
	ASSERT_EQ(0.0625f, texture.getTileHeightGL());	
}

TEST_F(TextureWithDefaultValidTextureInfo, getTileHeightGL_AssignedByConstructorWith16And16_Return1)
{
	textureInfo.height = 16;
	Texture texture(textureInfo);
	ASSERT_EQ(1.0f, texture.getTileHeightGL());	
}

TEST_F(TextureWithDefaultValidTextureInfo, getTextureInfo_ReturnValidTextureInfo)
{
	Texture texture(textureInfo);
	TextureInfo actual { texture.getTextureInfo() };
	ASSERT_EQ("file", actual.name);	
	ASSERT_EQ("file.png", actual.filename);	
	ASSERT_EQ(512, actual.width);	
	ASSERT_EQ(256, actual.height);	
	ASSERT_EQ(32, actual.tileWidth);	
	ASSERT_EQ(16, actual.tileHeight);	
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

TEST_F(TextureWithDefaultValidTextureInfo, setName_WhiteSpaces_ThrowInvalidArgument)
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

TEST_F(TextureWithDefaultValidTextureInfo, setFilename_Empty_ThrowInvalidArgument)
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

TEST_F(TextureWithDefaultValidTextureInfo, setFilename_WhiteSpaces_ThrowInvalidArgument)
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

TEST_F(TextureWithDefaultValidTextureInfo, setwidth_WithZero_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setWidth(0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setwidth_WithMinus1_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setWidth(-1);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setwidth_LessThanTileWidth_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setWidth(31);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be less than the width.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setwidth_With1024_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setWidth(1024);
	ASSERT_EQ(1024, texture.getWidth());
	ASSERT_EQ(0.03125f, texture.getTileWidthGL());
}

TEST_F(TextureWithDefaultValidTextureInfo, setHeight_WithZero_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setHeight(0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setHeight_WithMinus1_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setHeight(-1);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setHeight_LessThanTileHeight_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setHeight(15);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be less than the height.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setHeight_With1024_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setHeight(1024);
	ASSERT_EQ(1024, texture.getHeight());
	ASSERT_EQ(0.015625f, texture.getTileHeightGL());
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileWidth_WithZero_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileWidth(0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileWidth_WithMinus1_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileWidth(-1);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileWidth_GreaterThanWidth_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileWidth(513);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile width must be less than the width.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileWidth_With64_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setTileWidth(64);
	ASSERT_EQ(64, texture.getTileWidth());
	ASSERT_EQ(0.125f, texture.getTileWidthGL());
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileHeight_WithZero_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileHeight(0);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileHeight_WithMinus1_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileHeight(-1);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be greater than zero.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileHeight_GreaterThanHeight_ThrowInvalidArgument)
{
	try
	{
		Texture texture(textureInfo);
		texture.setTileHeight(257);
		FAIL();
	}
	catch(invalid_argument &err) 
	{
        ASSERT_STREQ("tile height must be less than the height.", err.what());
	}
}

TEST_F(TextureWithDefaultValidTextureInfo, setTileHeight_With64_ReturnSuccess)
{
	Texture texture(textureInfo);
	texture.setTileHeight(64);
	ASSERT_EQ(64, texture.getTileHeight());
	ASSERT_EQ(0.25f, texture.getTileHeightGL());

}
