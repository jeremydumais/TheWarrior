#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"
#include "glComponentController.hpp"

using mapeditor::controllers::EditConversationScenarioFormController;
using mapeditor::controllers::GLComponentController;
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

TEST(EditConversationScenarioFormController_getResourcesPath, ReturnConfiguredResourcesPath) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "resources/maps",
        std::nullopt,
        {});

    ASSERT_EQ("resources/maps", controller.getResourcesPath());
}

TEST(EditConversationScenarioFormController_getLastError, WithNewControllerReturnEmpty) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getLastError().empty());
}

TEST(EditConversationScenarioFormController_isEditMode, WithoutSelectedScenarioReturnFalse) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {});

    ASSERT_FALSE(controller.isEditMode());
}

TEST(EditConversationScenarioFormController_isEditMode, WithSelectedScenarioReturnTrue) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario(),
        {});

    ASSERT_TRUE(controller.isEditMode());
}

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed, WithUsedScenarioIdReturnTrueCaseInsensitively) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {"intro", "ending"});

    ASSERT_TRUE(controller.isScenarioIdAlreadyUsed("INTRO"));
}

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed, WithUnusedScenarioIdReturnFalse) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {"intro", "ending"});

    ASSERT_FALSE(controller.isScenarioIdAlreadyUsed("battle"));
}

TEST(EditConversationScenarioFormController_isScenarioIdAlreadyUsed, WithSelectedScenarioIdReturnFalseCaseInsensitively) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario(),
        {"scenario", "ending"});

    ASSERT_FALSE(controller.isScenarioIdAlreadyUsed("SCENARIO"));
}

TEST(EditConversationScenarioFormController_getAlreadyUsedNodeIds, WithoutSelectedScenarioReturnEmpty) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {});

    ASSERT_TRUE(controller.getAlreadyUsedNodeIds().empty());
}

TEST(EditConversationScenarioFormController_getAlreadyUsedNodeIds, WithSelectedScenarioReturnNodeIdsInOrder) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario({
            createDialogueNode("welcome"),
            createDialogueNode("goodbye")
        }),
        {});

    const std::vector<std::string> expectedIds = {"welcome", "goodbye"};
    ASSERT_EQ(expectedIds, controller.getAlreadyUsedNodeIds());
}

TEST(EditConversationScenarioFormController_getNodeById, WithMatchingNodeReturnNode) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
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

TEST(EditConversationScenarioFormController_getNodeById, WithoutMatchingNodeReturnEmpty) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.getNodeById("missing").has_value());
}

TEST(EditConversationScenarioFormController_getNodeById, WithoutSelectedScenarioReturnEmpty) {
    GLComponentController glComponentController;
    const EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        std::nullopt,
        {});

    ASSERT_FALSE(controller.getNodeById("welcome").has_value());
}

TEST(EditConversationScenarioFormController_addConversationNode, AddNodeToEndOfNodes) {
    GLComponentController glComponentController;
    EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    controller.addConversationNode(createDialogueNode("goodbye"));

    ASSERT_EQ(2, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
    EXPECT_EQ("goodbye", controller.getNodes()[1].getId());
}

TEST(EditConversationScenarioFormController_updateConversationNode, WithMatchingNodeReplaceNodeAndReturnTrue) {
    GLComponentController glComponentController;
    EditConversationScenarioFormController controller(
        &glComponentController,
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

TEST(EditConversationScenarioFormController_updateConversationNode, WithoutMatchingNodeReturnFalseAndLeaveNodesUnchanged) {
    GLComponentController glComponentController;
    EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.updateConversationNode(
        "missing",
        createDialogueNode("introduction")));

    ASSERT_EQ(1, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
}

TEST(EditConversationScenarioFormController_removeConversationNode, WithMatchingNodeRemoveNodeAndReturnTrue) {
    GLComponentController glComponentController;
    EditConversationScenarioFormController controller(
        &glComponentController,
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

TEST(EditConversationScenarioFormController_removeConversationNode, WithoutMatchingNodeReturnFalseAndLeaveNodesUnchanged) {
    GLComponentController glComponentController;
    EditConversationScenarioFormController controller(
        &glComponentController,
        "",
        createScenario({createDialogueNode("welcome")}),
        {});

    ASSERT_FALSE(controller.removeConversationNode("missing"));

    ASSERT_EQ(1, controller.getNodes().size());
    EXPECT_EQ("welcome", controller.getNodes()[0].getId());
}
