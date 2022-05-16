#include "item.hpp"
#include "itemType.hpp"
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

void itemSetIdWithInvalidArgument(Item &item,
                                  const string &id,
                                  const string &errorMessage)
{
    try {
        item.setId(id);
        FAIL();
    }
    catch(const invalid_argument &err) {
        ASSERT_STREQ(errorMessage.c_str(), err.what());
    }
}

void itemSetNameWithInvalidArgument(Item &item,
                                    const string &name,
                                    const string &errorMessage)
{
    try {
        item.setName(name);
        FAIL();
    }
    catch(const invalid_argument &err) {
        ASSERT_STREQ(errorMessage.c_str(), err.what());
    }
}

void itemSetTextureNameWithInvalidArgument(Item &item,
                                           const string &name,
                                           const string &errorMessage)
{
    try {
        item.setTextureName(name);
        FAIL();
    }
    catch(const invalid_argument &err) {
        ASSERT_STREQ(errorMessage.c_str(), err.what());
    }
}

void itemSetTextureIndexWithInvalidArgument(Item &item,
                                            const int index,
                                            const string &errorMessage)
{
    try {
        item.setTextureIndex(index);
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

TEST_F(ValidItemSample1, getType_ReturnItem) 
{
    ASSERT_EQ(ItemType::Item, item.getType());
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

TEST_F(ValidItemSample1, setId_WithEmpty_ThrowInvalidArgument) 
{
    itemSetIdWithInvalidArgument(item, "", "id cannot be empty.");
}

TEST_F(ValidItemSample1, setId_WithWhiteSpaces_ThrowInvalidArgument) 
{
    itemSetIdWithInvalidArgument(item, "  ", "id cannot be empty.");
}

TEST_F(ValidItemSample1, setId_WithLengthOfThree_ThrowInvalidArgument) 
{
    itemSetIdWithInvalidArgument(item, "abc", "id must be 6 characters long.");
}

TEST_F(ValidItemSample1, setId_WithLengthOfSeven_ThrowInvalidArgument) 
{
    itemSetIdWithInvalidArgument(item, "abc1234", "id must be 6 characters long.");
}

TEST_F(ValidItemSample1, setId_WithLengthOfSix_ReturnSuccess) 
{
    string expected { "abc123" };
    item.setId(expected);
    ASSERT_EQ(expected, item.getId());
}

TEST_F(ValidItemSample1, setName_WithEmpty_ThrowInvalidArgument) 
{
    itemSetNameWithInvalidArgument(item, "", "name cannot be empty.");
}

TEST_F(ValidItemSample1, setName_WithWhiteSpaces_ThrowInvalidArgument) 
{
    itemSetNameWithInvalidArgument(item, "  ", "name cannot be empty.");
}

TEST_F(ValidItemSample1, setName_WithValid_ReturnSuccess) 
{
    string expected { "Iron Shield" };
    item.setName(expected);
    ASSERT_EQ(expected, item.getName());
}

TEST_F(ValidItemSample1, setTextureName_WithEmpty_ThrowInvalidArgument) 
{
    itemSetTextureNameWithInvalidArgument(item, "", "textureName cannot be empty.");
}

TEST_F(ValidItemSample1, setTextureName_WithWhiteSpaces_ThrowInvalidArgument) 
{
    itemSetTextureNameWithInvalidArgument(item, "  ", "textureName cannot be empty.");
}

TEST_F(ValidItemSample1, setTextureName_WithValid_ReturnSuccess) 
{
    string expected { "tex456" };
    item.setTextureName(expected);
    ASSERT_EQ(expected, item.getTextureName());
}

TEST_F(ValidItemSample1, setTextureIndex_WithTextureIndexMinus1_ThrowInvalidArgument)
{
    itemSetTextureIndexWithInvalidArgument(item, -1, "textureIndex must be a positive number.");
}

TEST_F(ValidItemSample1, setTextureIndex_WithZero_ReturnSuccess) 
{
    int expected { 0 };
    item.setTextureIndex(expected);
    ASSERT_EQ(expected, item.getTextureIndex());
}

TEST_F(ValidItemSample1, equalityOperator_WithIdenticalItem_ReturnTrue)
{
    ASSERT_EQ(item, Item({
			"shd001",
            "Wooden Shield",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, equalityOperator_WithDifferenId_ReturnFalse)
{
    ASSERT_FALSE(item == Item({
			"shd002",
            "Wooden Shield",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, equalityOperator_WithDifferenName_ReturnFalse)
{
    ASSERT_FALSE(item == Item({
			"shd001",
            "Wooden ShieldS",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, equalityOperator_WithDifferenTextureName_ReturnFalse)
{
    ASSERT_FALSE(item == Item({
			"shd001",
            "Wooden Shield",
			"tex2",
            1
	}));
}

TEST_F(ValidItemSample1, equalityOperator_WithDifferenTextureIndex_ReturnFalse)
{
    ASSERT_FALSE(item == Item({
			"shd001",
            "Wooden Shield",
			"tex1",
            2
	}));
}

TEST_F(ValidItemSample1, inequalityOperator_WithIdenticalItem_ReturnFalse)
{
    ASSERT_FALSE(item != Item({
			"shd001",
            "Wooden Shield",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, inequalityOperator_WithDifferenId_ReturnTrue)
{
    ASSERT_NE(item, Item({
			"shd002",
            "Wooden Shield",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, inequalityOperator_WithDifferenName_ReturnTrue)
{
    ASSERT_NE(item, Item({
			"shd001",
            "Wooden ShieldS",
			"tex1",
            1
	}));
}

TEST_F(ValidItemSample1, inequalityOperator_WithDifferenTextureName_ReturnTrue)
{
    ASSERT_NE(item, Item({
			"shd001",
            "Wooden Shield",
			"tex2",
            1
	}));
}

TEST_F(ValidItemSample1, inequalityOperator_WithDifferenTextureIndex_ReturnTrue)
{
    ASSERT_NE(item, Item({
			"shd001",
            "Wooden Shield",
			"tex1",
            2
	}));
}