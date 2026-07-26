#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationDialogueFormController.hpp"

using mapeditor::controllers::EditConversationDialogueFormController;
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

TEST(EditConversationDialogueFormController_getLastError,
     WithNewControllerReturnEmpty) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getLastError().empty());
}

TEST(EditConversationDialogueFormController_isEditMode,
     WithoutSelectedNodeReturnFalse) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isEditMode());
}

TEST(EditConversationDialogueFormController_isEditMode,
     WithSelectedNodeReturnTrue) {
    const EditConversationDialogueFormController controller(
        createDialogueNode("welcome"),
        {});

    ASSERT_TRUE(controller.isEditMode());
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WithEmptyUsedNodeIdsReturnFalse) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WithDifferentNodeIdReturnFalse) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("reward"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WithMatchingNodeIdReturnTrue) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WithMatchingNodeIdDifferentCaseReturnTrue) {
    const EditConversationDialogueFormController controller(
        std::nullopt,
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("WELCOME"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WhenEditingWithOwnNodeIdReturnFalse) {
    const EditConversationDialogueFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("welcome"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WhenEditingWithOwnNodeIdDifferentCaseReturnFalse) {
    const EditConversationDialogueFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_FALSE(controller.isNodeIdAlreadyUsed("WELCOME"));
}

TEST(EditConversationDialogueFormController_isNodeIdAlreadyUsed,
     WhenEditingWithAnotherUsedNodeIdReturnTrue) {
    const EditConversationDialogueFormController controller(
        createDialogueNode("welcome"),
        {"welcome", "goodbye"});

    ASSERT_TRUE(controller.isNodeIdAlreadyUsed("goodbye"));
}
