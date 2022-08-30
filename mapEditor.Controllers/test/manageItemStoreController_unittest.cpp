#include "manageItemStoreController.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

using namespace mapeditor::controllers;

std::string getResourcesPathSample()
{
    return "/home/test/theWarrior/resources";
}

std::string getUserConfigFolderSample()
{
    return "/home/test/theWarrior/resources";
}

class ManageItemStoreControllerEmptySample : public ::testing::Test
{
public:
    ManageItemStoreControllerEmptySample()
        : controller(getResourcesPathSample(), getUserConfigFolderSample())
    {}
    ManageItemStoreController controller;
};

class ManageItemStoreControllerTwoStoresSample : public ::testing::Test
{
public:
    ManageItemStoreControllerTwoStoresSample()
        : controller(getResourcesPathSample(),
                     getUserConfigFolderSample(),
                     { { .name = "n1", .filename = "f1" },
                       { .name = "n2", .filename = "f2" } })
    {}
    ManageItemStoreController controller;
};

TEST(ManageItemStoreController_Constructor, WithEmptyResourcePath_ThrowInvalidArgument)
{
    try {
       ManageItemStoreController controller("", "test");
       FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("resourcesPath cannot be empty", err.what());
    }
}

TEST(ManageItemStoreController_Constructor, WithWhiteSpacesResourcePath_ThrowInvalidArgument)
{
    try {
       ManageItemStoreController controller("  ", "test");
       FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("resourcesPath cannot be empty", err.what());
    }
}

TEST(ManageItemStoreController_Constructor, WithEmptyUserConfigFolder_ThrowInvalidArgument)
{
    try {
       ManageItemStoreController controller("test", "");
       FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("userConfigFolder cannot be empty", err.what());
    }
}

TEST(ManageItemStoreController_Constructor, WithWhiteSpacesUserConfigFolder_ThrowInvalidArgument)
{
    try {
       ManageItemStoreController controller("test", "  ");
       FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("userConfigFolder cannot be empty", err.what());
    }
}

TEST(ManageItemStoreController_Constructor, WithValidValues_ReturnSuccess)
{
    ManageItemStoreController controller("test", "test2");
    ASSERT_EQ("test", controller.getResourcesPath());
    ASSERT_EQ("test2", controller.getUserConfigFolder());
    ASSERT_EQ(0, controller.getItemStores().size());
}

TEST_F(ManageItemStoreControllerEmptySample, GetResourcesPath_ReturnValidPath)
{
    ASSERT_EQ(getResourcesPathSample(), controller.getResourcesPath());
}

TEST_F(ManageItemStoreControllerEmptySample, GetUserConfigFolder_ReturnValidPath)
{
    ASSERT_EQ(getUserConfigFolderSample(), controller.getUserConfigFolder());
}

TEST_F(ManageItemStoreControllerEmptySample, GetItemStores_ReturnEmpty)
{
    ASSERT_EQ(0, controller.getItemStores().size());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, GetItemStores_Return2ItemStores)
{
    ASSERT_EQ(2, controller.getItemStores().size());
    ASSERT_EQ("n1", controller.getItemStores()[0].name);
    ASSERT_EQ("f1", controller.getItemStores()[0].filename);
    ASSERT_EQ("n2", controller.getItemStores()[1].name);
    ASSERT_EQ("f2", controller.getItemStores()[1].filename);
}

TEST_F(ManageItemStoreControllerEmptySample, AddItemStore_WithEmptyName_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "", .filename = "f1" }));
    ASSERT_EQ("name cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerEmptySample, AddItemStore_WithWhiteSpacesName_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "  ", .filename = "f1" }));
    ASSERT_EQ("name cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerEmptySample, AddItemStore_WithEmptyFilename_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "n1", .filename = "" }));
    ASSERT_EQ("filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerEmptySample, AddItemStore_WithWhiteSpacesFilename_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "n1", .filename = "   " }));
    ASSERT_EQ("filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerEmptySample, AddItemStore_WithValidInfo_ReturnTrue)
{
    ASSERT_EQ(0, controller.getItemStores().size());
    ASSERT_TRUE(controller.addItemStore({ .name = "n1", .filename = "f1" }));
    ASSERT_EQ(1, controller.getItemStores().size());
    ASSERT_EQ("n1", controller.getItemStores()[0].name);
    ASSERT_EQ("f1", controller.getItemStores()[0].filename);
}

TEST_F(ManageItemStoreControllerTwoStoresSample, AddItemStore_WithExistingName_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "n1", .filename = "f1" }));
    ASSERT_EQ("name n1 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, AddItemStore_WithExistingNameExtraSpaces_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "   n1   ", .filename = "f1" }));
    ASSERT_EQ("name n1 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, AddItemStore_WithExistingNameExtraSpacesAndCaps_ReturnFalse)
{
    ASSERT_FALSE(controller.addItemStore({ .name = "   N1  ", .filename = "f1" }));
    ASSERT_EQ("name N1 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithNonExistingName_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("T1", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name T1 doesn't exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithExistingNameButAlreadyUsed_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "N2", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithExistingNameWithSpacesAlreadyUsed_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "  N2  ", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithExistingNameWithSpacesAndUpperCaseAlreadyUsed_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "  N2  ", .filename = "F3" }));
    ASSERT_EQ("name N2 already exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithEmptyNameToEdit_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name to edit cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithWhiteSpacesNameToEdit_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("  ", { .name = "N3", .filename = "F3" }));
    ASSERT_EQ("name to edit cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithEmptyUpdatedName_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "", .filename = "F3" }));
    ASSERT_EQ("updated name cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithWhiteSpacesUpdatedName_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "   ", .filename = "F3" }));
    ASSERT_EQ("updated name cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithEmptyUpdatedFilename_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "N3", .filename = "" }));
    ASSERT_EQ("updated filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithWhiteSpacesUpdatedFilename_ReturnFalse)
{
    ASSERT_FALSE(controller.updateItemStore("N1", { .name = "N3", .filename = "   " }));
    ASSERT_EQ("updated filename cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithSameNameWithSpacesAndLowerCase_ReturnTrue)
{
    ASSERT_TRUE(controller.updateItemStore("N1", { .name = "  n1  ", .filename = "F3" }));
    const auto &itemStore = controller.getItemStores();
    ASSERT_EQ(2, itemStore.size());
    ASSERT_EQ("n1", itemStore[0].name);
    ASSERT_EQ("F3", itemStore[0].filename);
    ASSERT_EQ("n2", itemStore[1].name);
    ASSERT_EQ("f2", itemStore[1].filename);
}

TEST_F(ManageItemStoreControllerTwoStoresSample, UpdateItemStore_WithNonExistingName_ReturnTrue)
{
    ASSERT_TRUE(controller.updateItemStore("N1", { .name = "  n3  ", .filename = "F3" }));
    const auto &itemStore = controller.getItemStores();
    ASSERT_EQ(2, itemStore.size());
    ASSERT_EQ("n3", itemStore[0].name);
    ASSERT_EQ("F3", itemStore[0].filename);
    ASSERT_EQ("n2", itemStore[1].name);
    ASSERT_EQ("f2", itemStore[1].filename);
}

TEST_F(ManageItemStoreControllerTwoStoresSample, DeleteItemStore_WithEmptyName_ReturnFalse)
{
    ASSERT_FALSE(controller.deleteItemStore(""));
    ASSERT_EQ("name to delete cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, DeleteItemStore_WithWhiteSpacesName_ReturnFalse)
{
    ASSERT_FALSE(controller.deleteItemStore("  "));
    ASSERT_EQ("name to delete cannot be empty.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, DeleteItemStore_WithNonExistingName_ReturnFalse)
{
    ASSERT_FALSE(controller.deleteItemStore("N3"));
    ASSERT_EQ("name N3 doesn't exist.", controller.getLastError());
}

TEST_F(ManageItemStoreControllerTwoStoresSample, DeleteItemStore_WithExactExistingName_ReturnTrue)
{
    ASSERT_TRUE(controller.deleteItemStore("n1"));
    const auto &itemStore = controller.getItemStores();
    ASSERT_EQ(1, itemStore.size());
    ASSERT_EQ("n2", itemStore[0].name);
    ASSERT_EQ("f2", itemStore[0].filename);
}

TEST_F(ManageItemStoreControllerTwoStoresSample, DeleteItemStore_WithExistingNameSpacesAndCaps_ReturnTrue)
{
    ASSERT_TRUE(controller.deleteItemStore("  N2  "));
    const auto &itemStore = controller.getItemStores();
    ASSERT_EQ(1, itemStore.size());
    ASSERT_EQ("n1", itemStore[0].name);
    ASSERT_EQ("f1", itemStore[0].filename);
}
