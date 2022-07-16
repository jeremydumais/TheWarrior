#include "monster.hpp"
#include <gtest/gtest.h>

MonsterCreationInfo getMonsterInfoSample1()
{
    return {
        "Slime",
        "Tex1",
        1,
        6,
        1.0F,
        1.1F,
        0,
        2
    };
}

class MonsterSample1 : public ::testing::Test
{
public:
    MonsterSample1()
        : monster(getMonsterInfoSample1()) {}
    Monster monster;
};

TEST(Monster_Constructor, WithMonsterSample1_ReturnSuccess)
{
    Monster monster1(getMonsterInfoSample1());
}

TEST(Monster_Constructor, WithEmptyName_ThrowInvalidArgument)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.name = "";
    try {
        Monster monster1(creationInfo);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Monster_Constructor, WithWhiteSpacesName_ThrowInvalidArgument)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.name = "   ";
    try {
        Monster monster1(creationInfo);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Monster_Constructor, WithEmptyTextureName_ThrowInvalidArgument)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.textureName = "";
    try {
        Monster monster1(creationInfo);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture name cannot be null or empty.", err.what());
	}
}

TEST(Monster_Constructor, WithWhiteSpacesTextureName_ThrowInvalidArgument)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.textureName = "   ";
    try {
        Monster monster1(creationInfo);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture name cannot be null or empty.", err.what());
	}
}

TEST(Monster_Constructor, WithMinus1TextureIndex_ThrowInvalidArgument)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.textureIndex = -1;
    try {
        Monster monster1(creationInfo);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture index cannot be a negative number.", err.what());
	}
}

TEST(Monster_Constructor, WithTextureIndex0_ReturnSuccess)
{
    auto creationInfo = getMonsterInfoSample1();
    creationInfo.textureIndex = 0;
    Monster monster1(creationInfo);
}

TEST_F(MonsterSample1, GetName_WithMonsterSample1_ReturnSlime)
{
    ASSERT_EQ("Slime", monster.getName());
}

TEST_F(MonsterSample1, GetTextureName_WithMonsterSample1_ReturnSlime)
{
    ASSERT_EQ("Tex1", monster.getTextureName());
}

TEST_F(MonsterSample1, GetTextureIndex_WithMonsterSample1_Return1)
{
    ASSERT_EQ(1, monster.getTextureIndex());
}

TEST_F(MonsterSample1, GetHealth_WithMonsterSample1_Return1)
{
    ASSERT_EQ(6, monster.getHealth());
}

TEST_F(MonsterSample1, GetAttack_WithMonsterSample1_Return1F)
{
    ASSERT_FLOAT_EQ(1.0F, monster.getAttack());
}

TEST_F(MonsterSample1, GetDefense_WithMonsterSample1_Return1_1F)
{
    ASSERT_FLOAT_EQ(1.1F, monster.getDefense());
}

TEST_F(MonsterSample1, GetGoldRewardRange_WithMonsterSample1_Return0And2)
{
    std::pair<int, int> expected = { 0, 2 };
    ASSERT_EQ(expected, monster.getGoldRewardRange());
}

TEST_F(MonsterSample1, SetName_WithEmptyName_ThrowInvalidArgument)
{
    try {
        monster.setName("");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterSample1, SetName_WithWhiteSpacesName_ThrowInvalidArgument)
{
    try {
        monster.setName("  ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterSample1, SetName_WithTest_ReturnSuccess)
{
    const std::string NEW_NAME = "Test";
    monster.setName(NEW_NAME);
    ASSERT_EQ(NEW_NAME, monster.getName());
}

TEST_F(MonsterSample1, SetTextureName_WithEmptyTextureName_ThrowInvalidArgument)
{
    try {
        monster.setTextureName("");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterSample1, SetTextureName_WithWhiteSpacesTextureName_ThrowInvalidArgument)
{
    try {
        monster.setTextureName("  ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterSample1, SetTextureName_WithTest_ReturnSuccess)
{
    const std::string NEW_NAME = "Test";
    monster.setTextureName(NEW_NAME);
    ASSERT_EQ(NEW_NAME, monster.getTextureName());
}

TEST_F(MonsterSample1, SetTextureIndex_WithMinus1TextureIndex_ThrowInvalidArgument)
{
    try {
        monster.setTextureIndex(-1);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("texture index cannot be a negative number.", err.what());
	}
}

TEST_F(MonsterSample1, SetTextureIndex_WithTextureIndex5_ReturnSuccess)
{
    monster.setTextureIndex(5);
    ASSERT_EQ(5, monster.getTextureIndex());
}

TEST_F(MonsterSample1, SetHealth_With12_ReturnSuccess)
{
    monster.setHealth(12);
    ASSERT_EQ(12, monster.getHealth());
}

TEST_F(MonsterSample1, SetAttack_With1_6F_ReturnSuccess)
{
    monster.setAttack(1.6F);
    ASSERT_FLOAT_EQ(1.6F, monster.getAttack());
}

TEST_F(MonsterSample1, SetDefense_With1_6F_ReturnSuccess)
{
    monster.setDefense(1.6F);
    ASSERT_FLOAT_EQ(1.6F, monster.getDefense());
}

TEST_F(MonsterSample1, SetGoldRewardRange_With1And3_ReturnSuccess)
{
    monster.setGoldRewardRange(1, 3);
    std::pair<int, int> expected = { 1, 3 };
    ASSERT_EQ(expected, monster.getGoldRewardRange());
}

TEST_F(MonsterSample1, SetGoldRewardRange_WithMinus1And3_ThrowInvalidArgument)
{
    try {
        monster.setGoldRewardRange(-1, 3);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("gold reward minimum cannot be a negative number.", err.what());
	}
}

TEST_F(MonsterSample1, SetGoldRewardRange_With1And0_ThrowInvalidArgument)
{
    try {
        monster.setGoldRewardRange(1, 0);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("gold reward maximum must be greater or equal to the minimum.", err.what());
	}
}

TEST_F(MonsterSample1, SetGoldRewardRange_With1AndMinus5_ThrowInvalidArgument)
{
    try {
        monster.setGoldRewardRange(1, -5);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("gold reward maximum must be greater or equal to the minimum.", err.what());
	}
}
