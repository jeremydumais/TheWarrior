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
