#include <gtest/gtest.h>
#include <memory>
#include "gameMapModeController.hpp"
#include "itemFoundMessageDTO.hpp"
#include "npcDialogueMessageDTO.hpp"

using namespace thewarrior::ui::controllers;

TEST(GameMapModeController_addMessageToPipeline, WithMessageDTO_AddMessageToPipeline) {
    GameMapModeController controller;
    auto msg = std::make_unique<MessageDTO>();
    msg->message = "Hello";
    msg->maxDurationInMilliseconds = 2000;
    msg->scale = 0.7F;

    controller.addMessageToPipeline(std::move(msg));

    ASSERT_EQ(1, controller.getMessageCount());
    auto currentMessage = controller.getCurrentMessage();
    ASSERT_NE(nullptr, currentMessage);
    ASSERT_EQ(MessageDTOType::Message, currentMessage->getType());
    ASSERT_EQ("Hello", currentMessage->message);
    ASSERT_EQ(2000, currentMessage->maxDurationInMilliseconds);
    ASSERT_FLOAT_EQ(0.7F, currentMessage->scale);
}

TEST(GameMapModeController_addMessageToPipeline, WithItemFoundMessageDTO_AddItemFoundMessageToPipeline) {
    GameMapModeController controller;
    auto msg = std::make_unique<ItemFoundMessageDTO>();
    msg->message = "You found a sword!";
    msg->maxDurationInMilliseconds = 2000;
    msg->scale = 0.6F;
    msg->itemId = "ite001";
    msg->textureName = "items";

    controller.addMessageToPipeline(std::move(msg));

    ASSERT_EQ(1, controller.getMessageCount());
    auto currentMessage = controller.getCurrentMessage();
    ASSERT_NE(nullptr, currentMessage);
    ASSERT_EQ(MessageDTOType::ItemFoundMessage, currentMessage->getType());
    ASSERT_EQ("You found a sword!", currentMessage->message);
    ASSERT_EQ(2000, currentMessage->maxDurationInMilliseconds);
    ASSERT_FLOAT_EQ(0.6F, currentMessage->scale);
    auto *itemFoundMessage = dynamic_cast<ItemFoundMessageDTO *>(currentMessage.get());
    ASSERT_NE(nullptr, itemFoundMessage);
    ASSERT_EQ("ite001", itemFoundMessage->itemId);
    ASSERT_EQ("items", itemFoundMessage->textureName);
}

TEST(GameMapModeController_addMessageToPipeline, WithNPCDialogueMessageDTO_AddNPCDialogueMessageToPipeline) {
    GameMapModeController controller;
    auto msg = std::make_unique<NPCDialogueMessageDTO>();
    msg->message = "Hello traveler";
    msg->maxDurationInMilliseconds = 10000;
    msg->scale = 0.6F;
    msg->npcId = "npc001";

    controller.addMessageToPipeline(std::move(msg));

    ASSERT_EQ(1, controller.getMessageCount());
    auto currentMessage = controller.getCurrentMessage();
    ASSERT_NE(nullptr, currentMessage);
    ASSERT_EQ(MessageDTOType::NPCDialogueMessage, currentMessage->getType());
    ASSERT_EQ("Hello traveler", currentMessage->message);
    ASSERT_EQ(10000, currentMessage->maxDurationInMilliseconds);
    ASSERT_FLOAT_EQ(0.6F, currentMessage->scale);
    auto *npcDialogueMessage = dynamic_cast<NPCDialogueMessageDTO *>(currentMessage.get());
    ASSERT_NE(nullptr, npcDialogueMessage);
    ASSERT_EQ("npc001", npcDialogueMessage->npcId);
}
