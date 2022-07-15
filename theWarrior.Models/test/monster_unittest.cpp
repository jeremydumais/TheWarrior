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

TEST_F(MonsterSample1, GetGoldMinimum_WithMonsterSample1_Return0)
{
    ASSERT_EQ(0, monster.getGoldMinimum());
}

TEST_F(MonsterSample1, GetGoldMaximum_WithMonsterSample1_Return2)
{
    ASSERT_EQ(2, monster.getGoldMaximum());
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