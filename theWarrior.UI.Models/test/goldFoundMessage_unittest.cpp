#include <gtest/gtest.h>
#include <optional>
#include "goldFoundMessage.hpp"

using namespace thewarrior::ui::models;

class GoldFoundMessageSample : public ::testing::Test {
 public:
    GoldFoundMessageSample()
        : msg("You found 3 gold pieces!", 2000, 0.6F, 3U) {}

    GoldFoundMessage msg;
};

TEST(GoldFoundMessage_Constructor, WithValues_ReturnInitializedMessage) {
    GoldFoundMessage msg("You found 10 gold pieces!", 2500, 0.8F, 10U);

    ASSERT_EQ("You found 10 gold pieces!", msg.getMessage());
    ASSERT_EQ(2500, msg.getMaxDurationInMilliseconds());
    ASSERT_FLOAT_EQ(0.8F, msg.getScale());
    ASSERT_EQ(std::nullopt, msg.getDisplayStartTime());
    ASSERT_EQ(10U, msg.getGoldAmount());
}

TEST_F(GoldFoundMessageSample, getType_ReturnGoldFoundMessage) {
    ASSERT_EQ(MessageType::GoldFoundMessage, msg.getType());
}

TEST_F(GoldFoundMessageSample, getGoldAmount_Return3) {
    ASSERT_EQ(3U, msg.getGoldAmount());
}

TEST_F(GoldFoundMessageSample, setGoldAmount_With25_Return25) {
    msg.setGoldAmount(25U);

    ASSERT_EQ(25U, msg.getGoldAmount());
}
