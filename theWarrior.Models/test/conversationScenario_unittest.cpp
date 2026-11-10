#include <gtest/gtest.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <stdexcept>
#include "conversationScenario.hpp"

using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;
using thewarrior::models::ConversationAction;
using thewarrior::models::MerchantShopAction;
using thewarrior::models::MonsterFightAction;
using thewarrior::models::ProgressStoryLineAction;
using thewarrior::models::RestRequestedAction;
using thewarrior::models::RewardAction;
using thewarrior::models::SellItemsAction;

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

TEST(ConversationAction_Compatibility, PreserveExistingVariantIndexes) {
    EXPECT_EQ(0, ConversationAction(RewardAction {}).which());
    EXPECT_EQ(1, ConversationAction(ProgressStoryLineAction {}).which());
    EXPECT_EQ(2, ConversationAction(RestRequestedAction {}).which());
    EXPECT_EQ(3, ConversationAction(MerchantShopAction {}).which());
    EXPECT_EQ(4, ConversationAction(SellItemsAction {}).which());
    EXPECT_EQ(5, ConversationAction(MonsterFightAction {}).which());
}

TEST(MonsterFightAction_Serialization, PreserveMonsterId) {
    const ConversationAction expected = MonsterFightAction {
        .monsterId = "forest-troll"
    };
    std::stringstream serialized;
    {
        boost::archive::text_oarchive archive(serialized);
        archive << expected;
    }

    ConversationAction actual;
    {
        boost::archive::text_iarchive archive(serialized);
        archive >> actual;
    }

    ASSERT_EQ(expected.which(), actual.which());
    const auto *monsterFight = boost::get<MonsterFightAction>(&actual);
    ASSERT_NE(nullptr, monsterFight);
    EXPECT_EQ("forest-troll", monsterFight->monsterId);
}
