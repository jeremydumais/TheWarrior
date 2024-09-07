#include <boost/property_tree/ptree_fwd.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include "../include/configurationManager.hpp"
#include "iJSONFileStream.hpp"

using thewarrior::storage::ConfigurationManager;
using thewarrior::storage::IJSONFileStream;
using boost::property_tree::ptree;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::NiceMock;
using ::testing::_;

class StubJSONFileStream : public IJSONFileStream {
 public:
    explicit StubJSONFileStream(const std::string &fileName) : IJSONFileStream(fileName) {}
    MOCK_METHOD(const std::string &, getLastError, (), (const override));
    MOCK_METHOD(bool, fileExists, (), (const override));
    MOCK_METHOD(bool, readFile, (boost::property_tree::ptree &obj), (override));
    MOCK_METHOD(bool, writeFile, (const boost::property_tree::ptree &obj), (override));
};

TEST(ConfigurationManager_Constructor, fileNameEmpty_ThrowInvalidArgument) {
    try {
        ConfigurationManager config("");
        FAIL();
    } catch(std::invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
    }
}

TEST(ConfigurationManager_Constructor, fileNameWhiteSpaces_ThrowInvalidArgument) {
    try {
        ConfigurationManager config("   ");
        FAIL();
    } catch(std::invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
    }
}

TEST(ConfigurationManager_load, fileExistsIsFalse_ReturnFalse) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(false));

    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_FALSE(config.load());
    ASSERT_EQ("The file test doesn't exist.", config.getLastError());
}

TEST(ConfigurationManager_load, readFileIsFalse_ReturnFalse) {
    const std::string lastErrorSample { "This is an error" };
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, getLastError()).WillByDefault(ReturnRef(lastErrorSample));
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ON_CALL(*stubJFS, readFile(_)).WillByDefault(Return(false));

    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_FALSE(config.load());
    ASSERT_EQ("This is an error", config.getLastError());
}

TEST(ConfigurationManager_load, fileExistAndIsReadable_ReturnTrue) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ON_CALL(*stubJFS, readFile(_)).WillByDefault(Return(true));

    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
}

TEST(ConfigurationManager_save, writeFileIsFalse_ReturnFalse) {
    const std::string lastErrorSample { "This is an error" };

    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, getLastError()).WillByDefault(ReturnRef(lastErrorSample));
    ON_CALL(*stubJFS, writeFile(_)).WillByDefault(Return(false));
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_FALSE(config.save());
    ASSERT_EQ("This is an error", config.getLastError());
}

TEST(ConfigurationManager_save, writeFileIsTrue_ReturnTrue) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, writeFile(_)).WillByDefault(Return(true));
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.save());
}

TEST(ConfigurationManager_fileExists, fileExistsIsTrue_ReturnTrue) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.fileExists());
}

TEST(ConfigurationManager_fileExists, fileExistsIsFalse_ReturnFalse) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(false));
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_FALSE(config.fileExists());
}

void setupStubJSONFileStreamExpectations(NiceMock<StubJSONFileStream> &stubJFS,
                           ptree &expectedPTree) {
    ON_CALL(stubJFS, fileExists()).WillByDefault(Return(true));
    ON_CALL(stubJFS, readFile(_)).WillByDefault([&expectedPTree](ptree &obj) {
        obj = expectedPTree;
        return true;
    });
}

TEST(ConfigurationManager_getStringValue, WithExistingKeyString1_ReturnString2) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    expectedPTree.put("string1", "string2");
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_EQ("string2", config.getStringValue("string1"));
}

TEST(ConfigurationManager_getStringValue, WithInexistingKeyString1_ReturnEmpty) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_EQ("", config.getStringValue("string1"));
}

TEST(ConfigurationManager_getBoolValue, WithInexistingKeyBool1_ReturnFalse) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_FALSE(config.getBoolValue("bool1"));
}

TEST(ConfigurationManager_getBoolValue, WithExistingKeyBool1And1_ReturnTrue) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    expectedPTree.put("bool1", 1);
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_TRUE(config.getBoolValue("bool1"));
}

TEST(ConfigurationManager_getBoolValue, WithExistingKeyBool1And0_ReturnFalse) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    expectedPTree.put("bool1", 0);
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_FALSE(config.getBoolValue("bool1"));
}

TEST(ConfigurationManager_getBoolValue, WithExistingKeyBool1AndTrue_ReturnTrue) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    expectedPTree.put("bool1", true);
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_TRUE(config.getBoolValue("bool1"));
}

TEST(ConfigurationManager_getBoolValue, WithExistingKeyBool1AndFalse_ReturnFalse) {
    auto stubJFS = std::make_unique<NiceMock<StubJSONFileStream>>("test");
    ptree expectedPTree;
    expectedPTree.put("bool1", false);
    setupStubJSONFileStreamExpectations(*stubJFS, expectedPTree);
    ConfigurationManager config("test", std::move(stubJFS));
    ASSERT_TRUE(config.load());
    ASSERT_FALSE(config.getBoolValue("bool1"));
}

