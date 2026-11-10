#include <gtest/gtest.h>
#include "textureStamp.hpp"

using mapeditor::controllers::makeTextureStamp;
using mapeditor::controllers::makeTextureStampPreview;
using thewarrior::models::GameMap;
using thewarrior::models::Texture;

TEST(TextureStamp, KeepsAtlasLayoutWithBottomOriginIndicesAndRectangularTiles) {
    const Texture atlas({.name="atlas", .filename="atlas.png", .width=128, .height=96, .tileWidth=32, .tileHeight=16});
    const auto stamp = makeTextureStamp(atlas, 1, 2, 2, 3, false);
    EXPECT_EQ((std::vector<int>{13, 14, 9, 10, 5, 6}), stamp.indices);
    GameMap map(8, 8);
    const auto preview = makeTextureStampPreview(map, stamp, {3, 4});
    ASSERT_TRUE(preview);
    ASSERT_EQ(6U, preview->size());
    for (size_t i = 0; i < preview->size(); ++i) {
        EXPECT_EQ(stamp.indices[i], preview->at(i).getTextureIndex());
        EXPECT_EQ("atlas", preview->at(i).getTextureName());
    }
}

TEST(TextureStamp, TexturePreviewPreservesObjectAndTilePropertiesWithoutChangingMap) {
    const Texture atlas({"atlas", "atlas.png", 64, 64, 32, 32});
    GameMap map(4, 4);
    auto &target = map.getTileForEditing({1, 1});
    target.setTextureName("ground");
    target.setTextureIndex(4);
    target.setObjectTextureName("tree");
    target.setObjectTextureIndex(7);
    target.setCanPlayerSteppedOn(false);
    target.setObjectAbovePlayer(true);
    target.setAllowsInteractionThrough(true);
    target.setIsWallToClimb(true);
    target.setMonsterZoneIndex(2);
    const auto before = target;
    const auto stamp = makeTextureStamp(atlas, 0, 0, 1, 1, false);
    const auto preview = makeTextureStampPreview(map, stamp, {1, 1});
    ASSERT_TRUE(preview);
    auto expected = before;
    expected.setTextureName("atlas");
    expected.setTextureIndex(2);
    EXPECT_EQ(expected, preview->front());
    EXPECT_EQ(before, target);
}

TEST(TextureStamp, ObjectPreviewPreservesTextureAndUsesNewDestinationAfterMoving) {
    const Texture atlas({"atlas", "atlas.png", 64, 64, 32, 32});
    const auto stamp = makeTextureStamp(atlas, 0, 0, 2, 1, true);
    GameMap map(4, 4);
    auto &first = map.getTileForEditing({0, 0});
    first.setTextureName("grass");
    first.setTextureIndex(6);
    auto &second = map.getTileForEditing({2, 2});
    second.setTextureName("water");
    second.setTextureIndex(8);
    second.setCanPlayerSteppedOn(false);
    const auto firstBefore = first;
    const auto secondBefore = second;
    const auto initial = makeTextureStampPreview(map, stamp, {0, 0});
    const auto moved = makeTextureStampPreview(map, stamp, {2, 2});
    ASSERT_TRUE(initial);
    ASSERT_TRUE(moved);
    auto expected = secondBefore;
    expected.setObjectTextureName("atlas");
    expected.setObjectTextureIndex(2);
    EXPECT_EQ(expected, moved->front());
    EXPECT_EQ("grass", initial->front().getTextureName());
    EXPECT_EQ(firstBefore, first);
    EXPECT_EQ(secondBefore, second);
}

TEST(TextureStamp, RejectsOutOfBoundsPlacementWithoutWrappingOrClipping) {
    const Texture atlas({"atlas", "atlas.png", 64, 64, 32, 32});
    const auto stamp = makeTextureStamp(atlas, 0, 0, 2, 2, false);
    GameMap map(4, 4);
    EXPECT_TRUE(makeTextureStampPreview(map, stamp, {2, 2}));
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {3, 2}));
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {2, 3}));
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {-1, 0}));
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {0, -1}));
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {4, 4}));
}

TEST(TextureStamp, RejectsInvalidAtlasSelectionAndMalformedStamp) {
    const Texture atlas({"atlas", "atlas.png", 64, 64, 32, 32});
    EXPECT_TRUE(makeTextureStamp(atlas, 0, 0, 0, 1, false).indices.empty());
    EXPECT_TRUE(makeTextureStamp(atlas, 1, 0, 2, 1, false).indices.empty());
    EXPECT_TRUE(makeTextureStamp(atlas, 0, -1, 1, 1, false).indices.empty());
    GameMap map(4, 4);
    auto stamp = makeTextureStamp(atlas, 0, 0, 2, 2, false);
    stamp.indices.pop_back();
    EXPECT_FALSE(makeTextureStampPreview(map, stamp, {0, 0}));
}
