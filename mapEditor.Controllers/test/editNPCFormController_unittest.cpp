#include <gtest/gtest.h>
#include <qpixmap.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "editNPCFormController.hpp"
#include "iTexturePixmapProvider.hpp"
#include "npcDTO.hpp"

using commoneditor::ui::ITexturePixmapProvider;
using mapeditor::controllers::EditNPCFormController;
using mapeditor::controllers::NPCDTO;
using thewarrior::models::ConversationScenario;

class FakeTexturePixmapProvider final : public ITexturePixmapProvider {
 public:
    std::shared_ptr<QPixmap> loadPixmap(const std::string &) override {
        return nullptr;
    }
    ~FakeTexturePixmapProvider() override;
};

FakeTexturePixmapProvider::~FakeTexturePixmapProvider() {}

class EditNPCFormControllerNewNPC : public ::testing::Test {
 public:
    EditNPCFormControllerNewNPC()
    : controller("", {}, pixmapProvider, std::nullopt, idSamples) {}
    ~EditNPCFormControllerNewNPC() override;
    std::vector<std::string> idSamples { "NPC001", "NPC002" };
    FakeTexturePixmapProvider pixmapProvider;
    EditNPCFormController controller;
};

EditNPCFormControllerNewNPC::~EditNPCFormControllerNewNPC() {}

class EditNPCFormControllerEditNPC : public ::testing::Test {
 public:
    EditNPCFormControllerEditNPC()
    : controller("", {}, pixmapProvider, NPCDTO {"NPC002", "Joe Blow", "Tex1", 0}, idSamples) {}
    ~EditNPCFormControllerEditNPC() override;
    std::vector<std::string> idSamples { "NPC001", "NPC002" };
    FakeTexturePixmapProvider pixmapProvider;
    EditNPCFormController controller;
};

EditNPCFormControllerEditNPC::~EditNPCFormControllerEditNPC() {}

TEST_F(EditNPCFormControllerNewNPC, WithNPC003_ReturnFalse) {
    ASSERT_FALSE(controller.isNPCIdAlreadyUsed("NPC003"));
}

TEST_F(EditNPCFormControllerNewNPC, WithNPC001_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed("NPC001"));
}

TEST_F(EditNPCFormControllerNewNPC, WithNPC001LowerCase_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed("npc001"));
}

TEST_F(EditNPCFormControllerNewNPC, WithNPC001LowerCaseAndSpaces_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed(" npc001 "));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC003_ReturnFalse) {
    ASSERT_FALSE(controller.isNPCIdAlreadyUsed("NPC003"));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC002_ReturnFalse) {
    ASSERT_FALSE(controller.isNPCIdAlreadyUsed("NPC002"));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC002LowerCase_ReturnFalse) {
    ASSERT_FALSE(controller.isNPCIdAlreadyUsed("npc002"));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC002LowerCaseAndSpaces_ReturnFalse) {
    ASSERT_FALSE(controller.isNPCIdAlreadyUsed(" npc002 "));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC001_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed("NPC001"));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC001LowerCase_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed("npc001"));
}

TEST_F(EditNPCFormControllerEditNPC, WithNPC001LowerCaseAndSpaces_ReturnTrue) {
    ASSERT_TRUE(controller.isNPCIdAlreadyUsed(" npc001 "));
}

TEST(EditNPCFormController_getConversationScenarioById,
     WithMatchingScenarioReturnScenario) {
    FakeTexturePixmapProvider pixmapProvider;
    NPCDTO npc {
        .id = "NPC001",
        .conversationScenarios = {
            ConversationScenario("greeting", "", {}),
            ConversationScenario("farewell", "", {})
        }
    };
    const EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        npc,
        {});

    const auto scenario = controller.getConversationScenarioById("farewell");

    ASSERT_TRUE(scenario.has_value());
    ASSERT_EQ("farewell", scenario->getId());
}

TEST(EditNPCFormController_getConversationScenarioById,
     WithoutMatchingScenarioReturnEmpty) {
    FakeTexturePixmapProvider pixmapProvider;
    NPCDTO npc {
        .id = "NPC001",
        .conversationScenarios = {
            ConversationScenario("greeting", "", {})
        }
    };
    const EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        npc,
        {});

    ASSERT_FALSE(
        controller.getConversationScenarioById("missing").has_value());
}

TEST(EditNPCFormController_getConversationScenarioById,
     WithoutSelectedNPCReturnEmpty) {
    FakeTexturePixmapProvider pixmapProvider;
    const EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        std::nullopt,
        {});

    ASSERT_FALSE(
        controller.getConversationScenarioById("greeting").has_value());
}

TEST(EditNPCFormController_addConversationScenario,
     AddScenarioToController) {
    FakeTexturePixmapProvider pixmapProvider;
    EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        std::nullopt,
        {});

    controller.addConversationScenario(
        ConversationScenario("greeting", "", {}));

    ASSERT_TRUE(controller.removeConversationScenario("greeting"));
}

TEST(EditNPCFormController_updateConversationScenario,
     WithMatchingScenarioReplaceScenarioAndReturnTrue) {
    FakeTexturePixmapProvider pixmapProvider;
    NPCDTO npc {
        .id = "NPC001",
        .conversationScenarios = {
            ConversationScenario("greeting", "", {}),
            ConversationScenario("farewell", "", {})
        }
    };
    EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        npc,
        {});

    ASSERT_TRUE(controller.updateConversationScenario(
        "greeting",
        ConversationScenario("introduction", "", {})));
    EXPECT_FALSE(controller.removeConversationScenario("greeting"));
    EXPECT_TRUE(controller.removeConversationScenario("introduction"));
    EXPECT_TRUE(controller.removeConversationScenario("farewell"));
}

TEST(EditNPCFormController_updateConversationScenario,
     WithoutMatchingScenarioReturnFalse) {
    FakeTexturePixmapProvider pixmapProvider;
    EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        std::nullopt,
        {});

    ASSERT_FALSE(controller.updateConversationScenario(
        "missing",
        ConversationScenario("introduction", "", {})));
}

TEST(EditNPCFormController_removeConversationScenario,
     WithMatchingScenarioRemoveScenarioAndReturnTrue) {
    FakeTexturePixmapProvider pixmapProvider;
    NPCDTO npc {
        .id = "NPC001",
        .conversationScenarios = {
            ConversationScenario("greeting", "", {}),
            ConversationScenario("farewell", "", {})
        }
    };
    EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        npc,
        {});

    ASSERT_TRUE(controller.removeConversationScenario("greeting"));
    EXPECT_FALSE(controller.removeConversationScenario("greeting"));
    EXPECT_TRUE(controller.removeConversationScenario("farewell"));
}

TEST(EditNPCFormController_removeConversationScenario,
     WithoutMatchingScenarioReturnFalse) {
    FakeTexturePixmapProvider pixmapProvider;
    EditNPCFormController controller(
        "",
        {},
        pixmapProvider,
        std::nullopt,
        {});

    ASSERT_FALSE(controller.removeConversationScenario("missing"));
}
