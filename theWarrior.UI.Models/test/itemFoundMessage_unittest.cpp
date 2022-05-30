#include "itemFoundMessage.hpp"
#include <gtest/gtest.h>

class ItemFoundMessageSample : public ::testing::Test
{
public:
    ItemFoundMessageSample()
        : msg("Test", 2000, "ite001", "tex2")
    {}
    ItemFoundMessage msg;
};

TEST(ItemFoundMessage_MinimalConstructor, WithTestOnly_ReturnSuccess)
{
    auto message = "test";
    auto itemId = "ite002";
    ItemFoundMessage msg(message, 13, itemId, "tex3");
    ASSERT_EQ(message, msg.getMessage());
    ASSERT_EQ(13, msg.getMaxDurationInMilliseconds());
    ASSERT_EQ(std::nullopt, msg.getDisplayStartTime());
    ASSERT_EQ(itemId, msg.getItemId());
}

TEST(ItemFoundMessage_GetType, ReturnMessage)
{
    Message msg("test");
    ASSERT_EQ(MessageType::Message, msg.getType());
}

TEST_F(ItemFoundMessageSample, getItemId_ReturnIte001)
{
    ASSERT_EQ("ite001", msg.getItemId());
}

TEST_F(ItemFoundMessageSample, setItemId_ReturnSuccess)
{
    auto newId = "swd001";
    msg.setItemId(newId);
    ASSERT_EQ(newId, msg.getItemId());
}

TEST_F(ItemFoundMessageSample, getTextureName_ReturnTex2)
{
    ASSERT_EQ("tex2", msg.getTextureName());
}

TEST_F(ItemFoundMessageSample, setTextureName_ReturnSuccess)
{
    auto newTextureName = "tex8";
    msg.setTextureName(newTextureName);
    ASSERT_EQ(newTextureName, msg.getTextureName());
}