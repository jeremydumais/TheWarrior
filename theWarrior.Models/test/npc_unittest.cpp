#include <gtest/gtest.h>
#include <stdexcept>
#include "npc.hpp"
#include "point.hpp"

using namespace thewarrior::models;

NPCCreationInfo getNPCInfoSample1() {
    return {
        "npc001",
        "Joe Blow"
    };
}

class NPCSample1 : public ::testing::Test {
 public:
    NPCSample1()
        : npc(getNPCInfoSample1()) {}
    NPC npc;
};

TEST(NPC_Constructor, WithEmptyID_ThrowInvalidArgument) {
    try {
        NPC npc1({""});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, WithWhiteSpacesID_ThrowInvalidArgument) {
    try {
        NPC npc1({" "});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, With5CharsID_ThrowInvalidArgument) {
    try {
        NPC npc1({"NPC01"});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, WithEmptyName_ThrowInvalidArgument) {
    try {
        NPC npc1({"NPC001", ""});
        FAIL();
    } catch (const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be empty.", err.what());
    } catch (...) {
        FAIL();
    }
}

TEST(NPC_Constructor, WithWhiteSpacesName_ThrowInvalidArgument) {
    try {
        NPC npc1({"NPC001", " "});
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
    npc.setId("NPC002");
    ASSERT_EQ("NPC002", npc.getId());
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
    npc.setName("Test");
    ASSERT_EQ("Test", npc.getName());
}

