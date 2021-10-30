#include "item.hpp"
#include <gtest/gtest.h>

using namespace std;

class ItemNotCreated_ValidCreationInfo : public ::testing::Test
{
public:
	ItemNotCreated_ValidCreationInfo() 
	  : itemCreationInfo {
			"shd001",
            "Wooden Shield",
			"tex1",
            1
		} 
    {}
	ItemCreationInfo itemCreationInfo;
};

class ValidItemSample1 : public ::testing::Test
{
public:
	ValidItemSample1() 
	  : item({
			"shd001",
            "Wooden Shield",
			"tex1",
            1
		})
    {}
	Item item;
};


void itemCreationWithInvalidArgument(const ItemCreationInfo &itemCreationInfo,
                                     const string &errorMessage)
{
    try {
        Item item(itemCreationInfo);
        FAIL();
    }
    catch(const invalid_argument &err) {
        ASSERT_STREQ(errorMessage.c_str(), err.what());
    }
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithEmptyId_ThrowInvalidArgument)
{
    itemCreationInfo.id = "";
    itemCreationWithInvalidArgument(itemCreationInfo, "id cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithWhiteSpacesId_ThrowInvalidArgument)
{
    itemCreationInfo.id = "  ";
    itemCreationWithInvalidArgument(itemCreationInfo, "id cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithIdLengthOfThree_ThrowInvalidArgument)
{
    itemCreationInfo.id = "shd";
    itemCreationWithInvalidArgument(itemCreationInfo, "id must be 6 characters long.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithIdLengthOfSeven_ThrowInvalidArgument)
{
    itemCreationInfo.id = "shd123A";
    itemCreationWithInvalidArgument(itemCreationInfo, "id must be 6 characters long.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithIdLengthOfSix_ReturnSuccess)
{
    itemCreationInfo.id = "shd123";
    Item item(itemCreationInfo);
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithEmptyName_ThrowInvalidArgument)
{
    itemCreationInfo.name = "";
    itemCreationWithInvalidArgument(itemCreationInfo, "name cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithWhiteSpacesName_ThrowInvalidArgument)
{
    itemCreationInfo.name = "  ";
    itemCreationWithInvalidArgument(itemCreationInfo, "name cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithEmptyTextureName_ThrowInvalidArgument)
{
    itemCreationInfo.textureName = "";
    itemCreationWithInvalidArgument(itemCreationInfo, "textureName cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithWhiteSpacesTextureName_ThrowInvalidArgument)
{
    itemCreationInfo.textureName = "  ";
    itemCreationWithInvalidArgument(itemCreationInfo, "textureName cannot be empty.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_WithTextureIndexMinus1_ThrowInvalidArgument)
{
    itemCreationInfo.textureIndex = -1;
    itemCreationWithInvalidArgument(itemCreationInfo, "textureIndex must be a positive number.");
}

TEST_F(ItemNotCreated_ValidCreationInfo, Constructor_ValidInfos_ReturnSuccess)
{
    Item item(itemCreationInfo);
}

TEST_F(ValidItemSample1, getId_ReturnShd001) 
{
    ASSERT_EQ("shd001", item.getId());
}

TEST_F(ValidItemSample1, getName_ReturnWoodenShield) 
{
    ASSERT_EQ("Wooden Shield", item.getName());
}

TEST_F(ValidItemSample1, getTextureName_ReturnTex1) 
{
    ASSERT_EQ("tex1", item.getTextureName());
}

TEST_F(ValidItemSample1, getTextureIndex_Return1) 
{
    ASSERT_EQ(1, item.getTextureIndex());
}

/*TEST_F(ValidItemSample1, setIdWithEmpty_ThrowInvalidArgument) 
{
    ASSERT_EQ(1, item.getTextureIndex());
}*/