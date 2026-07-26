#include <gtest/gtest.h>
#include <stdexcept>
#include "conversationScenario.hpp"

using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

TEST(ConversationNodeTransition_nextInOrder, ReturnNextInOrderWithEmptyNodeId) {
    const auto transition = ConversationNodeTransition::nextInOrder();

    ASSERT_EQ(ConversationNodeTransitionType::NextInOrder, transition.getType());
    ASSERT_TRUE(transition.getNextNodeId().empty());
}

TEST(ConversationNodeTransition_stop, ReturnStopWithEmptyNodeId) {
    const auto transition = ConversationNodeTransition::stop();

    ASSERT_EQ(ConversationNodeTransitionType::Stop, transition.getType());
    ASSERT_TRUE(transition.getNextNodeId().empty());
}

TEST(ConversationNodeTransition_toNode, WithNodeIdReturnSpecificNode) {
    const auto transition = ConversationNodeTransition::toNode("reward");

    ASSERT_EQ(ConversationNodeTransitionType::SpecificNode, transition.getType());
    ASSERT_EQ("reward", transition.getNextNodeId());
}

TEST(ConversationNodeTransition_toNode, WithEmptyNodeIdThrowInvalidArgument) {
    ASSERT_THROW(
        ConversationNodeTransition::toNode(""),
        std::invalid_argument);
}

TEST(ConversationNode_Constructor, WithoutTransitionDefaultToNextInOrder) {
    const ConversationNode node(
        "welcome",
        ConversationDialogue { .lines = {"Welcome."} });

    ASSERT_EQ(ConversationNodeTransitionType::NextInOrder,
              node.getTransition().getType());
}

TEST(ConversationNode_setTransition, WithStopTransitionSetStop) {
    ConversationNode node(
        "welcome",
        ConversationDialogue { .lines = {"Welcome."} });

    node.setTransition(ConversationNodeTransition::stop());

    ASSERT_EQ(ConversationNodeTransitionType::Stop,
              node.getTransition().getType());
}
