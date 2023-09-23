#include "rgbItemColor.hpp"
#include <gtest/gtest.h>

using namespace thewarrior::models;

class RGBItemColorSample : public ::testing::Test
{
public:
    RGBItemColorSample()
        : itemColor("Red", "#ff0000")
    {}
    RGBItemColor itemColor;
};

TEST(RGBItemColor_Constructor, WithEmptyName_ThrowInvalidArgument)
{
	try {
        auto itemColor = RGBItemColor("", "#FFFFFF");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithWhiteSpacesName_ThrowInvalidArgument)
{
	try {
        auto itemColor = RGBItemColor("    ", "#FFFFFF");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithTestName_ReturnSuccess)
{
    ASSERT_NO_THROW(RGBItemColor("test", "#ffffff"));
}

TEST(RGBItemColor_Constructor, WithTestNameAndCapsFFFFFF_ReturnSuccess)
{
    auto itemColor = RGBItemColor("test", "#FFFFFF");
    ASSERT_EQ("test", itemColor.getName());
    ASSERT_EQ("#FFFFFF", itemColor.getValue());
}

TEST(RGBItemColor_Constructor, WithTestNameAnd012389_ReturnSuccess)
{
    auto itemColor = RGBItemColor("test", "#012389");
    ASSERT_EQ("test", itemColor.getName());
    ASSERT_EQ("#012389", itemColor.getValue());
}

TEST(RGBItemColor_Constructor, WithEmptyValue_ThrowInvalidArgument)
{
	try {
        auto itemColor = RGBItemColor("test", "");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value cannot be null or empty.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithWhiteSpacesValue_ThrowInvalidArgument)
{
	try {
        auto itemColor = RGBItemColor("test", "    ");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value cannot be null or empty.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithABValue_ThrowInvalidArgument)
{
	try	{
        auto itemColor = RGBItemColor("test", "AB");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must be in the following hexadecimal format #000000.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithPoundABValue_ThrowInvalidArgument)
{
	try	{
        auto itemColor = RGBItemColor("test", "#AB");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must be in the following hexadecimal format #000000.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithPoundABCDEFValue_ReturnSuccess)
{
    auto itemColor = RGBItemColor("test", "#ABCDEF");
    ASSERT_EQ("#ABCDEF", itemColor.getValue());
}

TEST(RGBItemColor_Constructor, WithPoundABCDEGValue_ThrowInvalidArgument)
{
	try	{
        auto itemColor = RGBItemColor("test", "#ABCDEG");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value has an invalid hexadecimal character : G.", err.what());
	}
}

TEST(RGBItemColor_Constructor, WithUnderscoreABCDEFValue_ThrowInvalidArgument)
{
	try	{
        auto itemColor = RGBItemColor("test", "_ABCDEF");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must start with the # char.", err.what());
	}
}

TEST_F(RGBItemColorSample, GetName_ReturnRed)
{
    ASSERT_EQ("Red", itemColor.getName());
}

TEST_F(RGBItemColorSample, GetValue_ReturnPoundFF0000)
{
    ASSERT_EQ("#FF0000", itemColor.getValue());
}

TEST_F(RGBItemColorSample, setNameWithEmptyString_ThrowInvalidArgument)
{
    try {
        itemColor.setName("");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST_F(RGBItemColorSample, setNameWithWhiteSpacesName_ThrowInvalidArgument)
{
    try {
        itemColor.setName("    ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST_F(RGBItemColorSample, setNameWithTest2Name_ReturnSuccess)
{
    itemColor.setName("Test2");
    ASSERT_EQ("Test2", itemColor.getName());
}

TEST_F(RGBItemColorSample, setValueWithCapsFFFFFF_ReturnSuccess)
{
    itemColor.setValue("#FFFFFF");
    ASSERT_EQ("#FFFFFF", itemColor.getValue());
}

TEST_F(RGBItemColorSample, setValuePound012389_ReturnSuccess)
{
    itemColor.setValue("#012389");
    ASSERT_EQ("#012389", itemColor.getValue());
}

TEST_F(RGBItemColorSample, setValueToEmptyString_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value cannot be null or empty.", err.what());
    }
}

TEST_F(RGBItemColorSample, setValueToWhiteSpacesString_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("    ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value cannot be null or empty.", err.what());
    }
}

TEST_F(RGBItemColorSample, setValueWithAB_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("AB");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must be in the following hexadecimal format #000000.", err.what());
    }
}

TEST_F(RGBItemColorSample, setValueWithPoundAB_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("#AB");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must be in the following hexadecimal format #000000.", err.what());
    }
}

TEST_F(RGBItemColorSample, setNameWithPoundABCDEFValue_ReturnSuccess)
{
    itemColor.setValue("#ABCDEF");
    ASSERT_EQ("#ABCDEF", itemColor.getValue());
}

TEST_F(RGBItemColorSample, setValueWithPoundABCDEG_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("#ABCDEG");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value has an invalid hexadecimal character : G.", err.what());
    }
}

TEST_F(RGBItemColorSample, setWithUnderscoreABCDEF_ThrowInvalidArgument)
{
    try {
        itemColor.setValue("_ABCDEF");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("value must start with the # char.", err.what());
    }
}

TEST_F(RGBItemColorSample, operatorEqualWithSameNameAndSameValue_ReturnTrue)
{
    ASSERT_EQ(itemColor, RGBItemColor("Red", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorEqualWithSameNameDifferentCaseAndSpacesAndSameValue_ReturnTrue)
{
    ASSERT_EQ(itemColor, RGBItemColor("  ReD  ", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorEqualWithSameNameAndSameValueDifferentCase_ReturnTrue)
{
    ASSERT_EQ(itemColor, RGBItemColor("Red", "#FF0000"));
}

TEST_F(RGBItemColorSample, operatorEqualWithDifferentName_ReturnFalse)
{
    ASSERT_FALSE(itemColor == RGBItemColor("ed", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorEqualWithDifferentValue_ReturnFalse)
{
    ASSERT_FALSE(itemColor == RGBItemColor("Red", "#fe0000"));
}

TEST_F(RGBItemColorSample, operatorNotEqualWithSameNameAndSameValue_ReturnFalse)
{
        ASSERT_FALSE(itemColor != RGBItemColor("Red", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorNotEqualWithSameNameDifferentCaseAndSpacesAndSameValue_ReturnFalse)
{
        ASSERT_FALSE(itemColor != RGBItemColor("  ReD  ", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorNotEqualWithSameNameAndSameValueDifferentCase_ReturnFalse)
{
        ASSERT_FALSE(itemColor != RGBItemColor("Red", "#FF0000"));
}

TEST_F(RGBItemColorSample, operatorNotEqualWithDifferentName_ReturnTrue)
{
        ASSERT_NE(itemColor, RGBItemColor("ed", "#ff0000"));
}

TEST_F(RGBItemColorSample, operatorNotEqualWithDifferentValue_ReturnTrue)
{
        ASSERT_NE(itemColor, RGBItemColor("Red", "#fe0000"));
}
