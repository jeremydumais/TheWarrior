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

class ManageItemStoreControllerSample : public ::testing::Test
{
public:
    ManageItemStoreControllerSample()
        : controller(getResourcesPathSample(), getUserConfigFolderSample())
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

TEST_F(ManageItemStoreControllerSample, GetResourcesPath_ReturnValidPath)
{
    ASSERT_EQ(getResourcesPathSample(), controller.getResourcesPath());
}

TEST_F(ManageItemStoreControllerSample, GetUserConfigFolder_ReturnValidPath)
{
    ASSERT_EQ(getUserConfigFolderSample(), controller.getUserConfigFolder());
}



