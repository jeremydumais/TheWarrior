#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"

using mapeditor::controllers::EditConversationNodeBaseFormController;
using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;

namespace {

ConversationNode createDialogueNode(const ConversationNodeId &id) {
    return ConversationNode(
        id,
        ConversationDialogue { .lines = {"Hello."} });
}

}  // namespace

TEST(EditConversationNodeBaseFormController_getLastError,
     WithNewControllerReturnEmpty) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getLastError().empty());
}

TEST(EditConversationNodeBaseFormController_isEditMode,
     WithoutSelectedNodeReturnFalse) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isEditMode());
}

TEST(EditConversationNodeBaseFormController_isEditMode,
     WithSelectedNodeReturnTrue) {
    const EditConversationNodeBaseFormController controller(
        createDialogueNode("welcome"),
        {});

    ASSERT_TRUE(controller.isEditMode());
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WithEmptyUsedNodeIdsReturnFalse) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WithDifferentNodeIdReturnFalse) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("reward"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WithMatchingNodeIdReturnTrue) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WithMatchingNodeIdDifferentCaseReturnTrue) {
    const EditConversationNodeBaseFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("WELCOME"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WhenEditingWithOwnNodeIdReturnFalse) {
    const EditConversationNodeBaseFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WhenEditingWithOwnNodeIdDifferentCaseReturnFalse) {
    const EditConversationNodeBaseFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("WELCOME"));
}

TEST(EditConversationNodeBaseFormController_isNodeIdAlreadyUsed,
     WhenEditingWithAnotherUsedNodeIdReturnTrue) {
    const EditConversationNodeBaseFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("goodbye"));
}
