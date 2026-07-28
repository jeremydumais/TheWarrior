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

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed,
     WithUsedScenarioIdReturnTrueCaseInsensitively) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {"intro", "ending"});

    ASSERT_TRUE(controller.isScenarioIdAlreadyUsed("INTRO"));
}

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed,
     WithUnusedScenarioIdReturnFalse) {
    const EditConversationScenarioFormController controller(
        "",
        std::nullopt,
        {"intro", "ending"});

    ASSERT_FALSE(controller.isScenarioIdAlreadyUsed("battle"));
}

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed,
     WithSelectedScenarioIdReturnFalseCaseInsensitively) {
    const EditConversationScenarioFormController controller(
        "",
        createScenario(),
        {"scenario", "ending"});

    ASSERT_FALSE(controller.isScenarioIdAlreadyUsed("SCENARIO"));
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

TEST(EditConversationScenarioFormController_addConversationNode,
     AddNodeToEndOfNodes) {
    EditConversationScenarioFormController controller(
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    controller.addConversationNode(createDialogueNode("goodbye"));

    ASSERT_EQ(2, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
    EXPECT_EQ("goodbye", controller.getNodes()[1].getId());
}

TEST(EditConversationScenarioFormController_updateConversationNode,
     WithMatchingNodeReplaceNodeAndReturnTrue) {
    EditConversationScenarioFormController controller(
        "",
        createScenario({
            createDialogueNode("welcome"),
            createDialogueNode("goodbye")
        }),
        {});

    ASSERT_TRUE(controller.updateConversationNode(
        "welcome",
        createDialogueNode("introduction")));

    ASSERT_EQ(2, controller.getNodes().size());
    EXPECT_EQ("introduction", controller.getNodes()[0].getId());
    EXPECT_EQ("goodbye", controller.getNodes()[1].getId());
}

TEST(EditConversationScenarioFormController_updateConversationNode,
     WithoutMatchingNodeReturnFalseAndLeaveNodesUnchanged) {
    EditConversationScenarioFormController controller(
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.updateConversationNode(
        "missing",
        createDialogueNode("introduction")));

    ASSERT_EQ(1, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
}

TEST(EditConversationScenarioFormController_removeConversationNode,
     WithMatchingNodeRemoveNodeAndReturnTrue) {
    EditConversationScenarioFormController controller(
        "",
        createScenario({
            createDialogueNode("welcome"),
            createDialogueNode("goodbye")
        }),
        {});

    ASSERT_TRUE(controller.removeConversationNode("welcome"));

    ASSERT_EQ(1, controller.getNodes().size());
    EXPECT_EQ("goodbye", controller.getNodes()[0].getId());
}

TEST(EditConversationScenarioFormController_removeConversationNode,
     WithoutMatchingNodeReturnFalseAndLeaveNodesUnchanged) {
    EditConversationScenarioFormController controller(
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.removeConversationNode("missing"));

    ASSERT_EQ(1, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
}
