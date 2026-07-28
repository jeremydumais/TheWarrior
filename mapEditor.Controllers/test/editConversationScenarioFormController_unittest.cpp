#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"

using mapeditor::controllers::EditConversationScenarioFormController;
using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationScenario;

namespace {

ConversationScenario createScenario(
        const std::vector<ConversationNode> &nodes = {}) {
    return ConversationScenario("scenario", "", nodes);
}

ConversationNode createDialogueNode(const std::string &id) {
    return ConversationNode(
        id,
        ConversationDialogue { .lines = {"Hello."} });
}

}  // namespace

TEST(EditConversationScenarioFormController_getResourcesPath,
     ReturnConfiguredResourcesPath) {
    const EditConversationScenarioFormController controller(
        "resources/maps",
        std::nullopt,
        {});

    ASSERT_EQ("resources/maps", controller.getResourcesPath());
}

TEST(EditConversationScenarioFormController_getLastError,
     WithNewControllerReturnEmpty) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getLastError().empty());
}

TEST(EditConversationScenarioFormController_isEditMode,
     WithoutSelectedScenarioReturnFalse) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isEditMode());
}

TEST(EditConversationScenarioFormController_isEditMode,
     WithSelectedScenarioReturnTrue) {
    const EditConversationScenarioFormController controller(
        "",
        createScenario(),
        {});

    ASSERT_TRUE(controller.isEditMode());
}

TEST(EditConversationScenarioFormController_getAlreadyUsedNodeIds,
     WithoutSelectedScenarioReturnEmpty) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getAlreadyUsedNodeIds().empty());
}

TEST(EditConversationScenarioFormController_getAlreadyUsedNodeIds,
     WithSelectedScenarioReturnNodeIdsInOrder) {
    const EditConversationScenarioFormController controller(
        "",
        createScenario({
            createDialogueNode("welcome"),
            createDialogueNode("goodbye")
        }),
        {});

    const std::vector<std::string> expectedIds = {"welcome", "goodbye"};
    ASSERT_EQ(expectedIds, controller.getAlreadyUsedNodeIds());
}

TEST(EditConversationScenarioFormController_getNodeById,
     WithMatchingNodeReturnNode) {
    const EditConversationScenarioFormController controller(
        "",
        createScenario({
            createDialogueNode("welcome"),
            createDialogueNode("goodbye")
        }),
        {});

    const auto &node = controller.getNodeById("goodbye");

    ASSERT_TRUE(node.has_value());
    ASSERT_EQ("goodbye", node->getId());
}

TEST(EditConversationScenarioFormController_getNodeById,
     WithoutMatchingNodeReturnEmpty) {
    const EditConversationScenarioFormController controller(
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.getNodeById("missing").has_value());
}

TEST(EditConversationScenarioFormController_getNodeById,
     WithoutSelectedScenarioReturnEmpty) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {});

    ASSERT_FALSE(controller.getNodeById("welcome").has_value());
}
