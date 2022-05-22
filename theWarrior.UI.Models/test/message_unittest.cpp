#include "message.hpp"
#include <gtest/gtest.h>

class MessageSample : public ::testing::Test
{
public:
    MessageSample()
        : msg("Test", 2000)
    {}
    Message msg;
};

TEST(Message_MinimalConstructor, WithTestOnly_ReturnSuccess)
{
    auto message = "test";
    Message msg(message);
    ASSERT_EQ(message, msg.getMessage());
    ASSERT_EQ(-1, msg.getMaxDurationInMilliseconds());
    ASSERT_EQ(std::nullopt, msg.getDisplayStartTime());
}

TEST(Message_GetType, ReturnMessage)
{
    Message msg("test");
    ASSERT_EQ(MessageType::Message, msg.getType());
}

TEST_F(MessageSample, getMessage_ReturnMessage)
{
    ASSERT_EQ("Test", msg.getMessage());
}

TEST_F(MessageSample, getMaxDurationInMilliseconds_Return2000)
{
    ASSERT_EQ(2000, msg.getMaxDurationInMilliseconds());
}

TEST_F(MessageSample, isDisplayed_ReturnFalse)
{
    ASSERT_FALSE(msg.isDisplayed());
}

TEST_F(MessageSample, hasMessageExpired_WithNotDisplayedMessage_ReturnFalse)
{
    auto currentTime = std::chrono::_V2::system_clock::now();
    ASSERT_FALSE(msg.isDisplayed());
    ASSERT_FALSE(msg.hasMessageExpired(currentTime));
}

TEST_F(MessageSample, hasMessageExpired_WithUnlimitedDuration_ReturnFalse)
{
    auto currentTime = std::chrono::_V2::system_clock::now();
    auto epoch = std::chrono::_V2::system_clock::from_time_t(0);
    msg.setMaxDurationInMilliseconds(-1);
    msg.setDisplayed(epoch);
    ASSERT_TRUE(msg.isDisplayed());
    ASSERT_FALSE(msg.hasMessageExpired(currentTime));
}

TEST_F(MessageSample, hasMessageExpired_WithDurationOf2000MsNotExpired_ReturnFalse)
{
    auto currentTime = std::chrono::_V2::system_clock::now();
    msg.setDisplayed(currentTime);
    ASSERT_TRUE(msg.isDisplayed());
    ASSERT_FALSE(msg.hasMessageExpired(currentTime));
}

TEST_F(MessageSample, hasMessageExpired_WithDurationOf2000MsNotExpiredBy1Ms_ReturnFalse)
{
    auto currentTime = std::chrono::_V2::system_clock::now();
    auto initialTime = currentTime - std::chrono::milliseconds(1999);
    msg.setDisplayed(initialTime);
    ASSERT_TRUE(msg.isDisplayed());
    ASSERT_FALSE(msg.hasMessageExpired(currentTime));
}

TEST_F(MessageSample, hasMessageExpired_WithDurationOf2000MsExpiredBy0Ms_ReturnTrue)
{
    auto currentTime = std::chrono::_V2::system_clock::now();
    auto initialTime = currentTime - std::chrono::milliseconds(2000);
    msg.setDisplayed(initialTime);
    ASSERT_TRUE(msg.isDisplayed());
    ASSERT_TRUE(msg.hasMessageExpired(currentTime));
}

TEST_F(MessageSample, setMessage_WithANewMsg_ReturnSuccess)
{
    auto expected = "A new msg";
    msg.setMessage(expected);
    ASSERT_EQ(expected, msg.getMessage());
}

TEST_F(MessageSample, setMaxDurationInMilliseconds_Minus1_ReturnSuccess)
{
    auto expected = -1;
    msg.setMaxDurationInMilliseconds(expected);
    ASSERT_EQ(expected, msg.getMaxDurationInMilliseconds());
}

TEST_F(MessageSample, setDisplayed_WithCurrentTime_ReturnSuccess)
{
    msg.setDisplayed(std::chrono::_V2::system_clock::now());
    ASSERT_TRUE(msg.isDisplayed());
}