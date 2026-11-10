#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>
#include "glNPC.hpp"
#include "npc.hpp"
#include "point.hpp"
#include "texture.hpp"

using namespace thewarrior::models;
using namespace thewarrior::ui;

namespace thewarrior::ui {

size_t getPointKey(const Point<size_t> &point);
Point<> toIntPoint(const Point<size_t> &point);
std::vector<Point<size_t>> getNeighbors(const Point<size_t> &point);

}  // namespace thewarrior::ui

namespace {

NPCCreationInfo getNPCInfo() {
    return {
        .id = "npc001",
        .name = "Joe",
        .textureName = "NPCTexture",
        .baseTextureIndex = 9,
        .spawnPosition = Point<size_t>(1, 2),
        .wanderZone = {
            Point<size_t>(1, 2),
            Point<size_t>(2, 2)
        },
        .dialogueLines = {
            "Hello"
        },
        .defaultFacing = NPCFacing::Left,
        .currentFacing = NPCFacing::Right,
        .defaultBehavior = NPCBehavior::Wander,
        .currentBehavior = NPCBehavior::Wander
    };
}

Texture getTexture() {
    return Texture({
        .name = "NPCTexture",
        .filename = "npc.png",
        .width = 384,
        .height = 256,
        .tileWidth = 32,
        .tileHeight = 32
    });
}

}  // namespace

TEST(GLNPC_Constructor, WithNPC_CopyNPCProperties) {
    NPC npc(getNPCInfo());
    const auto texture = getTexture();
    GLNPC glNPC(npc, texture);

    ASSERT_EQ("npc001", glNPC.getId());
    ASSERT_EQ("Joe", glNPC.getName());
    ASSERT_EQ("NPCTexture", glNPC.getTextureName());
    ASSERT_EQ(9, glNPC.getBaseTextureIndex());
    ASSERT_EQ(Point<size_t>(1, 2), glNPC.getSpawnPosition());
    ASSERT_EQ(NPCBehavior::Wander, glNPC.getCurrentBehavior());
    ASSERT_EQ(NPCFacing::Right, glNPC.getCurrentFacing());
}

TEST(GLNPC_isInMovement, WithDefaultConstructed_ReturnFalse) {
    NPC npc(getNPCInfo());
    const auto texture = getTexture();
    GLNPC glNPC(npc, texture);

    ASSERT_FALSE(glNPC.isInMovement());
}

TEST(GLNPC_initializeVisibility, StoryCompletionAfterInitialization_DoesNotChangeSnapshot) {
    auto info = getNPCInfo();
    info.visibilityRule = NPCVisibilityRule {
        .condition = NPCVisibilityCondition::NoStoryCompleted,
        .storyIds = {"guard-moved"}
    };
    NPC npc(info);
    const auto texture = getTexture();
    GLNPC glNPC(npc, texture);

    glNPC.initializeVisibility({});

    ASSERT_TRUE(glNPC.isCurrentlyVisible());
    ASSERT_FALSE(glNPC.isVisible({"guard-moved"}));
    glNPC.onCompletedStoryIdsChanged({"guard-moved"});
    ASSERT_TRUE(glNPC.isCurrentlyVisible());
}

TEST(GLNPC_onCompletedStoryIdsChanged, OnStoryChange_ReevaluateBothStoryConditions) {
    const auto texture = getTexture();
    for (const auto condition : {NPCVisibilityCondition::AnyStoryCompleted,
                                  NPCVisibilityCondition::NoStoryCompleted}) {
        SCOPED_TRACE(static_cast<int>(condition));
        auto info = getNPCInfo();
        info.visibilityRule = NPCVisibilityRule {
            .condition = condition,
            .storyIds = {"guard-moved"},
            .evaluationMode = NPCVisibilityEvaluationMode::OnStoryChange
        };
        GLNPC npc(NPC(info), texture);
        const bool initiallyVisible = condition == NPCVisibilityCondition::NoStoryCompleted;
        npc.initializeVisibility({});
        EXPECT_EQ(initiallyVisible, npc.isCurrentlyVisible());

        npc.onCompletedStoryIdsChanged({"unrelated"});
        EXPECT_EQ(initiallyVisible, npc.isCurrentlyVisible());
        npc.onCompletedStoryIdsChanged({"guard-moved"});
        EXPECT_EQ(!initiallyVisible, npc.isCurrentlyVisible());
        npc.onCompletedStoryIdsChanged({});
        EXPECT_EQ(initiallyVisible, npc.isCurrentlyVisible());
    }
}

TEST(GLNPC_onCompletedStoryIdsChanged, WithoutVisibilityRule_RemainVisible) {
    const auto texture = getTexture();
    GLNPC npc(NPC(getNPCInfo()), texture);
    npc.initializeVisibility({});

    npc.onCompletedStoryIdsChanged({"guard-moved"});

    EXPECT_TRUE(npc.isCurrentlyVisible());
}

TEST(GLNPC_onCompletedStoryIdsChanged, ScriptControlled_PreserveScriptVisibility) {
    auto info = getNPCInfo();
    info.visibilityRule = NPCVisibilityRule {
        .condition = NPCVisibilityCondition::ScriptControlled,
        .evaluationMode = NPCVisibilityEvaluationMode::OnStoryChange
    };
    const auto texture = getTexture();
    GLNPC npc(NPC(info), texture);
    npc.initializeVisibility({});
    npc.onCompletedStoryIdsChanged({"story1"});
    EXPECT_FALSE(npc.isCurrentlyVisible());

    npc.setScriptVisible(true);
    npc.onCompletedStoryIdsChanged({"story1", "story2"});
    EXPECT_TRUE(npc.isCurrentlyVisible());
}

TEST(GLNPC_setScriptVisible, WithScriptControlledNPC_UpdatesCurrentVisibility) {
    auto info = getNPCInfo();
    info.visibilityRule = NPCVisibilityRule {
        .condition = NPCVisibilityCondition::ScriptControlled
    };
    NPC npc(info);
    const auto texture = getTexture();
    GLNPC glNPC(npc, texture);
    glNPC.initializeVisibility({});

    ASSERT_FALSE(glNPC.isCurrentlyVisible());

    glNPC.setScriptVisible(true);

    ASSERT_TRUE(glNPC.isCurrentlyVisible());
}

TEST(GLNPC_stopWandering, WithWanderNPC_SetCurrentBehaviorToStationary) {
    NPC npc(getNPCInfo());
    const auto texture = getTexture();
    GLNPC glNPC(npc, texture);

    glNPC.stopWandering();

    ASSERT_EQ(NPCBehavior::Stationary, glNPC.getCurrentBehavior());
    ASSERT_EQ(NPCBehavior::Wander, glNPC.getDefaultBehavior());
}

TEST(GLNPC_getPointKey, WithSamePoint_ReturnSameKey) {
    ASSERT_EQ(getPointKey(Point<size_t>(2, 3)), getPointKey(Point<size_t>(2, 3)));
}

TEST(GLNPC_getPointKey, WithDifferentPoints_ReturnDifferentKeys) {
    ASSERT_NE(getPointKey(Point<size_t>(2, 3)), getPointKey(Point<size_t>(3, 2)));
}

TEST(GLNPC_toIntPoint, WithSizeTPoint_ReturnIntPoint) {
    ASSERT_EQ(Point<>(5, 7), toIntPoint(Point<size_t>(5, 7)));
}

TEST(GLNPC_getNeighbors, WithMiddlePoint_ReturnFourNeighbors) {
    const auto neighbors = getNeighbors(Point<size_t>(2, 2));
    const std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(2, 1),
        Point<size_t>(3, 2),
        Point<size_t>(2, 3)
    };

    ASSERT_EQ(expected, neighbors);
}

TEST(GLNPC_getNeighbors, WithOriginPoint_ReturnRightAndDownNeighbors) {
    const auto neighbors = getNeighbors(Point<size_t>(0, 0));
    const std::vector<Point<size_t>> expected {
        Point<size_t>(1, 0),
        Point<size_t>(0, 1)
    };

    ASSERT_EQ(expected, neighbors);
}
