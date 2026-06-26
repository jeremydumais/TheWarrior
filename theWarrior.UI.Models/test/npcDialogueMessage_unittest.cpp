#include <gtest/gtest.h>
#include "npcDialogueMessage.hpp"

using namespace thewarrior::ui::models;

class NPCDialogueMessageSample : public ::testing::Test {
 public:
    NPCDialogueMessageSample()
        : msg("Hello traveler", 10000, 0.6F, "npc001")
    {}
    NPCDialogueMessage msg;
};

TEST(NPCDialogueMessage_Constructor, WithValues_ReturnSuccess) {
    auto message = "Hello traveler";
    auto npcId = "npc002";
    NPCDialogueMessage msg(message, 10000, 0.6F, npcId);
    ASSERT_EQ(message, msg.getMessage());
    ASSERT_EQ(10000, msg.getMaxDurationInMilliseconds());
    ASSERT_FLOAT_EQ(0.6F, msg.getScale());
    ASSERT_EQ(std::nullopt, msg.getDisplayStartTime());
    ASSERT_EQ(npcId, msg.getNPCId());
}

TEST_F(NPCDialogueMessageSample, getType_ReturnNPCDialogueMessage) {
    ASSERT_EQ(MessageType::NPCDialogueMessage, msg.getType());
}

TEST_F(NPCDialogueMessageSample, getNPCId_ReturnNPC001) {
    ASSERT_EQ("npc001", msg.getNPCId());
}

TEST_F(NPCDialogueMessageSample, setNPCId_ReturnSuccess) {
    auto newId = "npc009";
    msg.setNPCId(newId);
    ASSERT_EQ(newId, msg.getNPCId());
}
