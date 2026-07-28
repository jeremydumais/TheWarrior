#include <gtest/gtest.h>
#include "conversationController.hpp"

using thewarrior::models::ConversationChoice;
using thewarrior::models::ConversationChoiceOption;
using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationScenario;
using namespace thewarrior::ui::controllers;

namespace {

ConversationNode dialogueNode(
    const std::string &id,
    const ConversationNodeTransition &transition =
        ConversationNodeTransition::nextInOrder()) {
    return ConversationNode(
        id,
        ConversationDialogue { .lines = {id} },
        transition);
}

ConversationScenario scenarioWith(
    std::vector<ConversationNode> nodes) {
    return ConversationScenario(
        "scenario",
        "",
        std::move(nodes));
}

}  // namespace

TEST(ConversationController_Constructor, ReturnInactiveController) {
    const ConversationController controller;

    EXPECT_EQ(ConversationState::Inactive, controller.getState());
    EXPECT_TRUE(controller.getNPCId().empty());
    EXPECT_TRUE(controller.getLastError().empty());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
}

TEST(ConversationController_start, WithValidScenarioStartAtFirstNode) {
    ConversationController controller;
    const auto scenario = scenarioWith({
        dialogueNode("welcome"),
        dialogueNode("goodbye")
    });

    ASSERT_TRUE(controller.start("npc001", scenario));

    EXPECT_EQ(ConversationState::Running, controller.getState());
    EXPECT_EQ("npc001", controller.getNPCId());
    EXPECT_TRUE(controller.getLastError().empty());
    ASSERT_NE(nullptr, controller.getCurrentNode());
    EXPECT_EQ("welcome", controller.getCurrentNode()->getId());
}

TEST(ConversationController_start, WithEmptyScenarioReturnError) {
    ConversationController controller;

    EXPECT_FALSE(controller.start(
        "npc001",
        scenarioWith({})));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
    EXPECT_FALSE(controller.getLastError().empty());
}

TEST(ConversationController_start, WithEmptyNodeIdReturnError) {
    ConversationController controller;

    EXPECT_FALSE(controller.start(
        "npc001",
        scenarioWith({dialogueNode("")})));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("empty id"));
}

TEST(ConversationController_start, WithDuplicateNodeIdsReturnError) {
    ConversationController controller;

    EXPECT_FALSE(controller.start(
        "npc001",
        scenarioWith({
            dialogueNode("welcome"),
            dialogueNode("welcome")
        })));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("duplicate"));
}

TEST(ConversationController_start, AfterErrorResetStateAndStartConversation) {
    ConversationController controller;
    ASSERT_FALSE(controller.start(
        "npc001",
        scenarioWith({})));

    ASSERT_TRUE(controller.start(
        "npc002",
        scenarioWith({dialogueNode("welcome")})));

    EXPECT_EQ(ConversationState::Running, controller.getState());
    EXPECT_EQ("npc002", controller.getNPCId());
    EXPECT_TRUE(controller.getLastError().empty());
}

TEST(ConversationController_stop, WithRunningConversationResetController) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({dialogueNode("welcome")})));

    controller.stop();

    EXPECT_EQ(ConversationState::Inactive, controller.getState());
    EXPECT_TRUE(controller.getNPCId().empty());
    EXPECT_TRUE(controller.getLastError().empty());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
}

TEST(ConversationController_completeCurrentNode, WithNextInOrderMoveToNextNode) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            dialogueNode("welcome"),
            dialogueNode("goodbye")
        })));

    ASSERT_TRUE(controller.completeCurrentNode());

    EXPECT_EQ(ConversationState::Running, controller.getState());
    ASSERT_NE(nullptr, controller.getCurrentNode());
    EXPECT_EQ("goodbye", controller.getCurrentNode()->getId());
}

TEST(ConversationController_completeCurrentNode, OnLastNodeCompleteConversation) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({dialogueNode("goodbye")})));

    ASSERT_TRUE(controller.completeCurrentNode());

    EXPECT_EQ(ConversationState::Completed, controller.getState());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
}

TEST(ConversationController_completeCurrentNode, WithStopTransitionCompleteConversation) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            dialogueNode(
                "welcome",
                ConversationNodeTransition::stop()),
            dialogueNode("unreachable")
        })));

    ASSERT_TRUE(controller.completeCurrentNode());

    EXPECT_EQ(ConversationState::Completed, controller.getState());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
}

TEST(ConversationController_completeCurrentNode, WithSpecificTransitionMoveToRequestedNode) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            dialogueNode(
                "welcome",
                ConversationNodeTransition::toNode("reward")),
            dialogueNode("skipped"),
            dialogueNode("reward")
        })));

    ASSERT_TRUE(controller.completeCurrentNode());

    EXPECT_EQ(ConversationState::Running, controller.getState());
    ASSERT_NE(nullptr, controller.getCurrentNode());
    EXPECT_EQ("reward", controller.getCurrentNode()->getId());
}

TEST(ConversationController_completeCurrentNode, WithMissingSpecificNodeReturnError) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            dialogueNode(
                "welcome",
                ConversationNodeTransition::toNode("missing"))
        })));

    EXPECT_FALSE(controller.completeCurrentNode());
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_EQ(nullptr, controller.getCurrentNode());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("missing"));
}

TEST(ConversationController_completeCurrentNode, WithoutActiveConversationReturnError) {
    ConversationController controller;

    EXPECT_FALSE(controller.completeCurrentNode());
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_FALSE(controller.getLastError().empty());
}

TEST(ConversationController_followTransition, WithoutActiveConversationReturnError) {
    ConversationController controller;

    EXPECT_FALSE(controller.followTransition(
        ConversationNodeTransition::stop()));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_FALSE(controller.getLastError().empty());
}

TEST(ConversationController_selectChoice, WithValidOptionMoveToOptionNode) {
    ConversationController controller;
    const ConversationChoice choice {
        .prompt = "Choose.",
        .options = {
            ConversationChoiceOption {
                .text = "First",
                .nextNodeId = "first"
            },
            ConversationChoiceOption {
                .text = "Second",
                .nextNodeId = "second"
            }
        }
    };
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            ConversationNode("choice", choice),
            dialogueNode("first"),
            dialogueNode("second")
        })));

    ASSERT_TRUE(controller.selectChoice(1));

    EXPECT_EQ(ConversationState::Running, controller.getState());
    ASSERT_NE(nullptr, controller.getCurrentNode());
    EXPECT_EQ("second", controller.getCurrentNode()->getId());
}

TEST(ConversationController_selectChoice, WithOutOfRangeOptionReturnError) {
    ConversationController controller;
    const ConversationChoice choice {
        .prompt = "Choose.",
        .options = {
            ConversationChoiceOption {
                .text = "First",
                .nextNodeId = "first"
            }
        }
    };
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            ConversationNode("choice", choice),
            dialogueNode("first")
        })));

    EXPECT_FALSE(controller.selectChoice(1));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("out of range"));
}

TEST(ConversationController_selectChoice, OnDialogueNodeReturnError) {
    ConversationController controller;
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({dialogueNode("welcome")})));

    EXPECT_FALSE(controller.selectChoice(0));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("not a choice"));
}

TEST(ConversationController_selectChoice, WithMissingDestinationReturnError) {
    ConversationController controller;
    const ConversationChoice choice {
        .prompt = "Choose.",
        .options = {
            ConversationChoiceOption {
                .text = "Missing",
                .nextNodeId = "missing"
            }
        }
    };
    ASSERT_TRUE(controller.start(
        "npc001",
        scenarioWith({
            ConversationNode("choice", choice)
        })));

    EXPECT_FALSE(controller.selectChoice(0));
    EXPECT_EQ(ConversationState::Error, controller.getState());
    EXPECT_NE(
        std::string::npos,
        controller.getLastError().find("missing"));
}
