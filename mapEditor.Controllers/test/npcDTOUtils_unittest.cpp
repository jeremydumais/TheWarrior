#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "npc.hpp"
#include "npcDTO.hpp"
#include "npcDTOUtils.hpp"
#include "point.hpp"
#include "story.hpp"

using mapeditor::controllers::NPCDTO;
using mapeditor::controllers::NPCDTOUtils;
using thewarrior::models::ConversationScenario;
using thewarrior::models::NPC;
using thewarrior::models::NPCBehavior;
using thewarrior::models::NPCCreationInfo;
using thewarrior::models::NPCFacing;
using thewarrior::models::NPCVisibilityCondition;
using thewarrior::models::NPCVisibilityRule;
using thewarrior::models::Point;

namespace mapeditor::controllers::npcdtoutils::unittest {

void assertConversationScenarios(
        const std::vector<ConversationScenario> &expected,
        const std::vector<ConversationScenario> &actual) {
    ASSERT_EQ(expected.size(), actual.size());
    for (size_t i = 0; i < expected.size(); i++) {
        ASSERT_EQ(expected[i].getId(), actual[i].getId());
        ASSERT_EQ(expected[i].getConditionalStoryIdCompleted(),
                  actual[i].getConditionalStoryIdCompleted());
        ASSERT_EQ(expected[i].getDialogueLines(),
                  actual[i].getDialogueLines());
        ASSERT_EQ(expected[i].getNodes().size(),
                  actual[i].getNodes().size());
    }
}

void assertNPCDTO(const NPCDTO &expected, const NPCDTO &actual) {
    ASSERT_EQ(expected.id, actual.id);
    ASSERT_EQ(expected.name, actual.name);
    ASSERT_EQ(expected.textureName, actual.textureName);
    ASSERT_EQ(expected.baseTextureIndex, actual.baseTextureIndex);
    ASSERT_EQ(expected.spawnPosition, actual.spawnPosition);
    ASSERT_EQ(expected.wanderZone, actual.wanderZone);
    assertConversationScenarios(expected.conversationScenarios,
                                actual.conversationScenarios);
    ASSERT_EQ(expected.defaultFacing, actual.defaultFacing);
    ASSERT_EQ(expected.currentFacing, actual.currentFacing);
    ASSERT_EQ(expected.defaultBehavior, actual.defaultBehavior);
    ASSERT_EQ(expected.currentBehavior, actual.currentBehavior);
    ASSERT_EQ(expected.visibilityRule, actual.visibilityRule);
}

void assertNPC(const NPCDTO &expected, const NPC &actual) {
    ASSERT_EQ(expected.id, actual.getId());
    ASSERT_EQ(expected.name, actual.getName());
    ASSERT_EQ(expected.textureName, actual.getTextureName());
    ASSERT_EQ(expected.baseTextureIndex, actual.getBaseTextureIndex());
    ASSERT_EQ(expected.spawnPosition, actual.getSpawnPosition());
    ASSERT_EQ(expected.wanderZone, actual.getWanderZone());
    assertConversationScenarios(expected.conversationScenarios,
                                actual.getConversationScenarios());
    ASSERT_EQ(expected.defaultFacing, actual.getDefaultFacing());
    ASSERT_EQ(expected.currentFacing, actual.getCurrentFacing());
    ASSERT_EQ(expected.defaultBehavior, actual.getDefaultBehavior());
    ASSERT_EQ(expected.currentBehavior, actual.getCurrentBehavior());
    ASSERT_EQ(expected.visibilityRule, actual.getVisibilityRule());
}

NPCDTO getNPCDTOSample1() {
    return {
        .id = "npc001",
        .name = "Joe Blow",
        .textureName = "Texture1",
        .baseTextureIndex = 9,
        .spawnPosition = Point<size_t>(1, 2),
        .wanderZone = {
            Point<size_t>(1, 2),
            Point<size_t>(2, 2),
            Point<size_t>(2, 3),
            Point<size_t>(3, 3)
        },
        .conversationScenarios = {
            ConversationScenario::fromLegacyDialogueLines({
                "Hello my name is Joe Blow",
                "How are you today?"
            })
        },
        .defaultFacing = NPCFacing::Left,
        .currentFacing = NPCFacing::Right,
        .defaultBehavior = NPCBehavior::Stationary,
        .currentBehavior = NPCBehavior::Wander,
        .visibilityRule = NPCVisibilityRule {
            .condition = NPCVisibilityCondition::AnyStoryCompleted,
            .storyIds = {"TEST_STORY_ID"}
        }
    };
}

TEST(npcDTOUtils_fromNPC, withNPC_ReturnValidDTO) {
    const auto expected = getNPCDTOSample1();
    const NPC npc(NPCCreationInfo {
        .id = expected.id,
        .name = expected.name,
        .textureName = expected.textureName,
        .baseTextureIndex = expected.baseTextureIndex,
        .spawnPosition = expected.spawnPosition,
        .wanderZone = expected.wanderZone,
        .dialogueLines = {},
        .conversationScenarios = expected.conversationScenarios,
        .defaultFacing = expected.defaultFacing,
        .currentFacing = expected.currentFacing,
        .defaultBehavior = expected.defaultBehavior,
        .currentBehavior = expected.currentBehavior,
        .visibilityRule = expected.visibilityRule
    });

    assertNPCDTO(expected, NPCDTOUtils::fromNPC(npc));
}

TEST(npcDTOUtils_toNPC, withValidDTO_ReturnSuccessWithNPC) {
    const auto dto = getNPCDTOSample1();

    const auto result = NPCDTOUtils::toNPC(dto);

    ASSERT_TRUE(result.success());
    ASSERT_TRUE(result.npc.has_value());
    ASSERT_EQ("", result.errorMessage);
    assertNPC(dto, result.npc.value());
}

TEST(npcDTOUtils_toNPC, withInvalidDTO_ReturnErrorMessage) {
    auto dto = getNPCDTOSample1();
    dto.id = "npc01";

    const auto result = NPCDTOUtils::toNPC(dto);

    ASSERT_FALSE(result.success());
    ASSERT_FALSE(result.npc.has_value());
    ASSERT_STREQ("id must be 6 characters long.", result.errorMessage.c_str());
}

}  // namespace mapeditor::controllers::npcdtoutils::unittest
