#include "manageMonsterStoreController.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

namespace mapeditor::controllers::managemonsterstorcontroller::unittest {

std::string getResourcesPathSample() {
    return "/home/test/theWarrior/resources";
}

std::string getUserConfigFolderSample() {
    return "/home/test/theWarrior/resources";
}

class ManageMonsterStoreControllerEmptySample : public ::testing::Test {
 public:
    ManageMonsterStoreControllerEmptySample()
        : controller(getResourcesPathSample(), getUserConfigFolderSample())
    {}
    ManageMonsterStoreController controller;
};

class ManageMonsterStoreControllerTwoStoresSample : public ::testing::Test {
 public:
    ManageMonsterStoreControllerTwoStoresSample()
        : controller(getResourcesPathSample(),
                     getUserConfigFolderSample(),
                     {
                         { .name = "n1", .filename = "f1" },
                         { .name = "n2", .filename = "f2" }
                     })
    {}
    ManageMonsterStoreController controller;
};

TEST(ManageMonsterStoreController_Constructor, WithEmptyResourcePath_ThrowInvalidArgument) {
    try {
       ManageMonsterStoreController controller("", "test");
       FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("resourcesPath cannot be empty", err.what());
    }
}

TEST(ManageMonsterStoreController_Constructor, WithWhiteSpacesResourcePath_ThrowInvalidArgument) {
    try {
       ManageMonsterStoreController controller("  ", "test");
       FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("resourcesPath cannot be empty", err.what());
    }
}

TEST(ManageMonsterStoreController_Constructor, WithEmptyUserConfigFolder_ThrowInvalidArgument) {
    try {
       ManageMonsterStoreController controller("test", "");
       FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("userConfigFolder cannot be empty", err.what());
    }
}

TEST(ManageMonsterStoreController_Constructor, WithWhiteSpacesUserConfigFolder_ThrowInvalidArgument) {
    try {
       ManageMonsterStoreController controller("test", "  ");
       FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("userConfigFolder cannot be empty", err.what());
    }
}

TEST(ManageMonsterStoreController_Constructor, WithValidValues_ReturnSuccess) {
    ManageMonsterStoreController controller("test", "test2");
    ASSERT_EQ("test", controller.getResourcesPath());
    ASSERT_EQ("test2", controller.getUserConfigFolder());
    ASSERT_EQ(0, controller.getMonsterStores().size());
}

TEST_F(ManageMonsterStoreControllerEmptySample, GetResourcesPath_ReturnValidPath) {
    ASSERT_EQ(getResourcesPathSample(), controller.getResourcesPath());
}

TEST_F(ManageMonsterStoreControllerEmptySample, GetUserConfigFolder_ReturnValidPath) {
    ASSERT_EQ(getUserConfigFolderSample(), controller.getUserConfigFolder());
}

TEST_F(ManageMonsterStoreControllerEmptySample, GetMonsterStores_ReturnEmpty) {
    ASSERT_EQ(0, controller.getMonsterStores().size());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, GetMonsterStores_Return2MonsterStores) {
    ASSERT_EQ(2, controller.getMonsterStores().size());
    ASSERT_EQ("n1", controller.getMonsterStores()[0].name);
    ASSERT_EQ("f1", controller.getMonsterStores()[0].filename);
    ASSERT_EQ("n2", controller.getMonsterStores()[1].name);
    ASSERT_EQ("f2", controller.getMonsterStores()[1].filename);
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, FindMonsterStore_WithN1LowerCase_Return1MonsterStore) {
    auto actual = controller.findMonsterStore("n1");
    ASSERT_TRUE(actual.has_value());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, FindMonsterStore_WithN1UpperCase_Return1MonsterStore) {
    auto actual = controller.findMonsterStore("N1");
    ASSERT_TRUE(actual.has_value());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, FindMonsterStore_WithN1UpperCaseAndSpaces_Return1MonsterStore) {
    auto actual = controller.findMonsterStore("  N1   ");
    ASSERT_TRUE(actual.has_value());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, FindMonsterStore_WithN3_ReturnNullOpt) {
    auto actual = controller.findMonsterStore("n3");
    ASSERT_FALSE(actual.has_value());
}

TEST_F(ManageMonsterStoreControllerEmptySample, AddMonsterStore_WithEmptyName_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "", .filename = "f1" }));
    ASSERT_EQ("name cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerEmptySample, AddMonsterStore_WithWhiteSpacesName_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "  ", .filename = "f1" }));
    ASSERT_EQ("name cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerEmptySample, AddMonsterStore_WithEmptyFilename_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "n1", .filename = "" }));
    ASSERT_EQ("filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerEmptySample, AddMonsterStore_WithWhiteSpacesFilename_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "n1", .filename = "   " }));
    ASSERT_EQ("filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerEmptySample, AddMonsterStore_WithValidInfo_ReturnTrue) {
    ASSERT_EQ(0, controller.getMonsterStores().size());
    ASSERT_TRUE(controller.addMonsterStore({ .name = "n1", .filename = "f1" }));
    ASSERT_EQ(1, controller.getMonsterStores().size());
    ASSERT_EQ("n1", controller.getMonsterStores()[0].name);
    ASSERT_EQ("f1", controller.getMonsterStores()[0].filename);
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, AddMonsterStore_WithExistingName_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "n1", .filename = "f1" }));
    ASSERT_EQ("name n1 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, AddMonsterStore_WithExistingNameExtraSpaces_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "   n1   ", .filename = "f1" }));
    ASSERT_EQ("name n1 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, AddMonsterStore_WithExistingNameExtraSpacesAndCaps_ReturnFalse) {
    ASSERT_FALSE(controller.addMonsterStore({ .name = "   N1  ", .filename = "f1" }));
    ASSERT_EQ("name N1 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithNonExistingName_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("T1", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name T1 doesn't exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithExistingNameButAlreadyUsed_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "N2", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithExistingNameWithSpacesAlreadyUsed_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "  N2  ", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithExistingNameWithSpacesAndUpperCaseAlreadyUsed_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "  N2  ", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithEmptyNameToEdit_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name to edit cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithWhiteSpacesNameToEdit_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("  ", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name to edit cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithEmptyUpdatedName_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "", .filename = "F3" }));
    ASSERT_EQ("updated name cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithWhiteSpacesUpdatedName_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "   ", .filename = "F3" }));
    ASSERT_EQ("updated name cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithEmptyUpdatedFilename_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "N3", .filename = "" }));
    ASSERT_EQ("updated filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithWhiteSpacesUpdatedFilename_ReturnFalse) {
    ASSERT_FALSE(controller.updateMonsterStore("N1", { .name = "N3", .filename = "   " }));
    ASSERT_EQ("updated filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithSameNameWithSpacesAndLowerCase_ReturnTrue) {
    ASSERT_TRUE(controller.updateMonsterStore("N1", { .name = "  n1  ", .filename = "F3" }));
    const auto &monsterStore = controller.getMonsterStores();
    ASSERT_EQ(2, monsterStore.size());
    ASSERT_EQ("n1", monsterStore[0].name);
    ASSERT_EQ("F3", monsterStore[0].filename);
    ASSERT_EQ("n2", monsterStore[1].name);
    ASSERT_EQ("f2", monsterStore[1].filename);
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, UpdateMonsterStore_WithNonExistingName_ReturnTrue) {
    ASSERT_TRUE(controller.updateMonsterStore("N1", { .name = "  n3  ", .filename = "F3" }));
    const auto &monsterStore = controller.getMonsterStores();
    ASSERT_EQ(2, monsterStore.size());
    ASSERT_EQ("n3", monsterStore[0].name);
    ASSERT_EQ("F3", monsterStore[0].filename);
    ASSERT_EQ("n2", monsterStore[1].name);
    ASSERT_EQ("f2", monsterStore[1].filename);
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, DeleteMonsterStore_WithEmptyName_ReturnFalse) {
    ASSERT_FALSE(controller.deleteMonsterStore(""));
    ASSERT_EQ("name to delete cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, DeleteMonsterStore_WithWhiteSpacesName_ReturnFalse) {
    ASSERT_FALSE(controller.deleteMonsterStore("  "));
    ASSERT_EQ("name to delete cannot be empty.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, DeleteMonsterStore_WithNonExistingName_ReturnFalse) {
    ASSERT_FALSE(controller.deleteMonsterStore("N3"));
    ASSERT_EQ("name N3 doesn't exist.", controller.getLastError());
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, DeleteMonsterStore_WithExactExistingName_ReturnTrue) {
    ASSERT_TRUE(controller.deleteMonsterStore("n1"));
    const auto &monsterStore = controller.getMonsterStores();
    ASSERT_EQ(1, monsterStore.size());
    ASSERT_EQ("n2", monsterStore[0].name);
    ASSERT_EQ("f2", monsterStore[0].filename);
}

TEST_F(ManageMonsterStoreControllerTwoStoresSample, DeleteMonsterStore_WithExistingNameSpacesAndCaps_ReturnTrue) {
    ASSERT_TRUE(controller.deleteMonsterStore("  N2  "));
    const auto &monsterStore = controller.getMonsterStores();
    ASSERT_EQ(1, monsterStore.size());
    ASSERT_EQ("n1", monsterStore[0].name);
    ASSERT_EQ("f1", monsterStore[0].filename);
}

}  // namespace mapeditor::controllers::managemonsterstorcontroller::unittest
