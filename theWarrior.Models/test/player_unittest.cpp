#include "player.hpp"
#include <gtest/gtest.h>

class PlayerSample : public ::testing::Test
{
public:
    PlayerSample()
        : player("player1")
    {}
    Player player;
};

TEST(Player_Constructor, WithEmptyName_ThrowInvalidArgument)
{
    try {
        Player actual("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Player_Constructor, WithWhitespacesName_ThrowInvalidArgument)
{
    try {
        Player actual("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Player_Constructor, WithJedName_ReturnSuccess)
{
    Player actual("Jed");
    ASSERT_EQ("Jed", actual.getName());
}

TEST_F(PlayerSample, getName_ReturnPlayer1) 
{
    ASSERT_EQ("player1", player.getName());
}

TEST_F(PlayerSample, getLevel_Return1) 
{
    ASSERT_EQ(1, player.getLevel());
}

TEST_F(PlayerSample, SetName_WithJohn_ReturnSuccess)
{
    auto expected = "John";
    player.setName(expected);
    ASSERT_EQ(expected, player.getName());
}

TEST_F(PlayerSample, SetName_WithEmptyName_ThrowInvalidArgument)
{
    try {
        player.setName("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(PlayerSample, SetName_WithWhitespacesName_ThrowInvalidArgument)
{
    try {
        player.setName("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(PlayerSample, SetLevel_With10_ReturnSuccess)
{
    auto expected = 10;
    player.setLevel(expected);
    ASSERT_EQ(expected, player.getLevel());
}

TEST_F(PlayerSample, IncrementLevel_ReturnSuccess)
{
    player.incrementLevel();
    ASSERT_EQ(2, player.getLevel());
}
