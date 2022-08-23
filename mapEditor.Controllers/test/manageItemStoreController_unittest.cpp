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
