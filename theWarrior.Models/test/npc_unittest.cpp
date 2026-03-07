#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "npc.hpp"
#include "point.hpp"

using thewarrior::models::NPC;
using thewarrior::models::NPCCreationInfo;
using thewarrior::models::NPCFacing;
using thewarrior::models::Point;

NPCCreationInfo getNPCInfoSample1() {
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
        .dialogueLines = {
            "Hello my name is Joe Blow",
            "How are you today?"
        },
        .defaultFacing = NPCFacing::Left,
        .currentFacing = NPCFacing::Right
    };
}

class NPCSample1 : public ::testing::Test {
 public:
    NPCSample1()
        : npc(getNPCInfoSample1()) {}
    ~NPCSample1() override;
    NPC npc;
};

NPCSample1::~NPCSample1() = default;

TEST(NPC_Constructor, With5CharsID_ThrowInvalidArgument) {
    try {
        NPC npc1({.id = "NPC01", .name = "Joe Blow"});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, WithEmptyName_ThrowInvalidArgument) {
    try {
        NPC npc1({.id = "NPC001", .name = ""});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, WithWhiteSpacesName_ThrowInvalidArgument) {
    try {
        NPC npc1({.id = "NPC001", .name = " "});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, getId_ReturnNPC001) {
    ASSERT_EQ("npc001", npc.getId());
}

TEST_F(NPCSample1, getName_ReturnJoeBlow) {
    ASSERT_EQ("Joe Blow", npc.getName());
}

TEST_F(NPCSample1, getTextureName_ReturnTexture1) {
    ASSERT_EQ("Texture1", npc.getTextureName());
}

TEST_F(NPCSample1, getBaseTextureIndex_Return9) {
    ASSERT_EQ(9, npc.getBaseTextureIndex());
}

TEST_F(NPCSample1, getSpawnPosition_ReturnJoeBlow) {
    ASSERT_EQ(Point<size_t>(1, 2), npc.getSpawnPosition());
}

TEST_F(NPCSample1, getWanderZone_Return4Points) {
    std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(2, 2),
        Point<size_t>(2, 3),
        Point<size_t>(3, 3)
    };

    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, getDialogueLines_Return2Lines) {
    std::vector<std::string> expected {
        "Hello my name is Joe Blow",
        "How are you today?"
    };
    ASSERT_EQ(expected, npc.getDialogueLines());
}

TEST_F(NPCSample1, getDefaultFacing_ReturnLeft) {
    ASSERT_EQ(NPCFacing::Left, npc.getDefaultFacing());
}

TEST_F(NPCSample1, getCurrentFacing_ReturnRight) {
    ASSERT_EQ(NPCFacing::Right, npc.getCurrentFacing());
}

TEST_F(NPCSample1, setId_WithWhiteSpaces_ThrowInvalidArgument) {
    try {
        npc.setId(" ");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, setId_WithEmpty_ThrowInvalidArgument) {
    try {
        npc.setId("");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, setId_With5CharsID_ThrowInvalidArgument) {
    try {
        npc.setId("NPC01");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, setId_WithNPC002_ReturnSuccess) {
    std::string expected("NPC002");
    npc.setId(expected);
    ASSERT_EQ(expected, npc.getId());
}

TEST_F(NPCSample1, setName_WithWhiteSpaces_ThrowInvalidArgument) {
    try {
        npc.setName(" ");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, setName_WithEmpty_ThrowInvalidArgument) {
    try {
        npc.setName("");
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST_F(NPCSample1, setName_WithTest_ReturnSuccess) {
    std::string expected("Test");
    npc.setName(expected);
    ASSERT_EQ(expected, npc.getName());
}

TEST_F(NPCSample1, setTextureName_WithTest_ReturnSuccess) {
    std::string expected("Test");
    npc.setTextureName(expected);
    ASSERT_EQ(expected, npc.getTextureName());
}

TEST_F(NPCSample1, setBaseTextureIndex_With8_ReturnSuccess) {
    int expected = 8;
    npc.setBaseTextureIndex(expected);
    ASSERT_EQ(expected, npc.getBaseTextureIndex());
}

TEST_F(NPCSample1, setSpawnPosition_With8_9_ReturnSuccess) {
    Point<size_t> expected(8, 9);
    npc.setSpawnPosition(expected);
    ASSERT_EQ(expected, npc.getSpawnPosition());
}

TEST_F(NPCSample1, setWanderZone_With2NewPoints_ReturnSuccess) {
    std::vector<Point<size_t>> expected {
        Point<size_t>(5, 6),
        Point<size_t>(6, 6)
    };
    npc.setWanderZone(expected);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, clearWanderZone_OnAFourPointsOne_ZoneCleared) {
    npc.clearWanderZone();
    ASSERT_TRUE(npc.getWanderZone().empty());
}

TEST_F(NPCSample1, addToWanderZone_With2NewPoints_TwoPointsAdded) {
    std::vector<Point<size_t>> newZones {
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(2, 2),
        Point<size_t>(2, 3),
        Point<size_t>(3, 3),
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    npc.addToWanderZone(newZones);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, addToWanderZone_With2NewPointsAndOneAlreadyExisting_TwoOfThreePointsAdded) {
    std::vector<Point<size_t>> newZones {
        Point<size_t>(4, 3),
        Point<size_t>(1, 2),
        Point<size_t>(5, 3)
    };
    std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(2, 2),
        Point<size_t>(2, 3),
        Point<size_t>(3, 3),
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    npc.addToWanderZone(newZones);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, addToWanderZone_With2PointsToEmptyZone_ZoneAdded) {
    std::vector<Point<size_t>> newZones {
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    std::vector<Point<size_t>> expected {
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    npc.clearWanderZone();
    npc.addToWanderZone(newZones);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, removeFromWanderZone_With2NonExistingPoints_NoChange) {
    std::vector<Point<size_t>> newZones {
        Point<size_t>(4, 3),
        Point<size_t>(5, 3)
    };
    std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(2, 2),
        Point<size_t>(2, 3),
        Point<size_t>(3, 3),
    };
    npc.removeFromWanderZone(newZones);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, removeFromWanderZone_With2ExistingPoints_TwoRemoved) {
    std::vector<Point<size_t>> zoneToRemove {
        Point<size_t>(2, 2),
        Point<size_t>(2, 3)
    };
    std::vector<Point<size_t>> expected {
        Point<size_t>(1, 2),
        Point<size_t>(3, 3),
    };
    npc.removeFromWanderZone(zoneToRemove);
    ASSERT_EQ(expected, npc.getWanderZone());
}

TEST_F(NPCSample1, removeFromWanderZone_WithAllPoints_ZoneCleared) {
    std::vector<Point<size_t>> zoneToRemove {
        Point<size_t>(1, 2),
        Point<size_t>(2, 2),
        Point<size_t>(2, 3),
        Point<size_t>(3, 3),
    };
    npc.removeFromWanderZone(zoneToRemove);
    ASSERT_TRUE(npc.getWanderZone().empty());
}

TEST_F(NPCSample1, setDialogueLines_With2NewLines_ReturnSuccess) {
    std::vector<std::string> expected {
        "This is a new line",
        "to test"
    };
    npc.setDialogueLines(expected);
    ASSERT_EQ(expected, npc.getDialogueLines());
}

TEST_F(NPCSample1, setDefaultFacing_WithUp_ReturnSuccess) {
    npc.setDefaultFacing(NPCFacing::Up);
    ASSERT_EQ(NPCFacing::Up, npc.getDefaultFacing());
}

TEST_F(NPCSample1, setCurrentFacing_WithUp_ReturnSuccess) {
    npc.setCurrentFacing(NPCFacing::Up);
    ASSERT_EQ(NPCFacing::Up, npc.getCurrentFacing());
}

TEST_F(NPCSample1, applyCoordinateOffset_WithXMinus1_ReturnSuccess) {
    npc.applyCoordinateOffset(-1, 0);
    ASSERT_EQ(Point<size_t>(0, 2), npc.getSpawnPosition());
    const auto wanderZone = npc.getWanderZone();
    ASSERT_EQ(Point<size_t>(0, 2), wanderZone.at(0));
    ASSERT_EQ(Point<size_t>(1, 2), wanderZone.at(1));
    ASSERT_EQ(Point<size_t>(1, 3), wanderZone.at(2));
    ASSERT_EQ(Point<size_t>(2, 3), wanderZone.at(3));
}

TEST_F(NPCSample1, applyCoordinateOffset_WithXPlus2_ReturnSuccess) {
    npc.applyCoordinateOffset(2, 0);
    ASSERT_EQ(Point<size_t>(3, 2), npc.getSpawnPosition());
    const auto wanderZone = npc.getWanderZone();
    ASSERT_EQ(Point<size_t>(3, 2), wanderZone.at(0));
    ASSERT_EQ(Point<size_t>(4, 2), wanderZone.at(1));
    ASSERT_EQ(Point<size_t>(4, 3), wanderZone.at(2));
    ASSERT_EQ(Point<size_t>(5, 3), wanderZone.at(3));
}

TEST_F(NPCSample1, applyCoordinateOffset_WithYMinus1_ReturnSuccess) {
    npc.applyCoordinateOffset(0, -1);
    ASSERT_EQ(Point<size_t>(1, 1), npc.getSpawnPosition());
    const auto wanderZone = npc.getWanderZone();
    ASSERT_EQ(Point<size_t>(1, 1), wanderZone.at(0));
    ASSERT_EQ(Point<size_t>(2, 1), wanderZone.at(1));
    ASSERT_EQ(Point<size_t>(2, 2), wanderZone.at(2));
    ASSERT_EQ(Point<size_t>(3, 2), wanderZone.at(3));
}

TEST_F(NPCSample1, applyCoordinateOffset_WithYPlus2_ReturnSuccess) {
    npc.applyCoordinateOffset(0, 2);
    ASSERT_EQ(Point<size_t>(1, 4), npc.getSpawnPosition());
    const auto wanderZone = npc.getWanderZone();
    ASSERT_EQ(Point<size_t>(1, 4), wanderZone.at(0));
    ASSERT_EQ(Point<size_t>(2, 4), wanderZone.at(1));
    ASSERT_EQ(Point<size_t>(2, 5), wanderZone.at(2));
    ASSERT_EQ(Point<size_t>(3, 5), wanderZone.at(3));
}
