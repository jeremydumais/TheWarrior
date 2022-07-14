#include "../include/configurationManager.hpp"
#include "iJSONFileStream.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::NiceMock;
using ::testing::_;

class StubJSONFileStream : public IJSONFileStream
{
public:
    explicit StubJSONFileStream(const string &fileName) : IJSONFileStream(fileName) {}
    MOCK_METHOD(const string &, getLastError, (), (const override));
    MOCK_METHOD(bool, fileExists, (), (const override));
    MOCK_METHOD(bool, readFile, (boost::property_tree::ptree &obj), (override));
    MOCK_METHOD(bool, writeFile, (const boost::property_tree::ptree &obj), (override));
};

TEST(ConfigurationManager_Constructor, fileNameEmpty_ThrowInvalidArgument)
{
    try {
        ConfigurationManager config("");
		FAIL();
    }
    catch(invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
	}
}

TEST(ConfigurationManager_Constructor, fileNameWhiteSpaces_ThrowInvalidArgument)
{
    try {
        ConfigurationManager config("   ");
		FAIL();
    }
    catch(invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
	}
}

TEST(ConfigurationManager_load, fileExistsIsFalse_ReturnFalse)
{
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(false));

    ConfigurationManager config("test", move(stubJFS));
    ASSERT_FALSE(config.load());
    ASSERT_EQ("The file test doesn't exist.", config.getLastError());
}

TEST(ConfigurationManager_load, readFileIsFalse_ReturnFalse)
{ 
    const string lastErrorSample { "This is an error" };
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, getLastError()).WillByDefault(ReturnRef(lastErrorSample));
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ON_CALL(*stubJFS, readFile(_)).WillByDefault(Return(false));

    ConfigurationManager config("test", move(stubJFS));
    ASSERT_FALSE(config.load());
    ASSERT_EQ("This is an error", config.getLastError());
}

TEST(ConfigurationManager_load, fileExistAndIsReadable_ReturnTrue)
{ 
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ON_CALL(*stubJFS, readFile(_)).WillByDefault(Return(true));

    ConfigurationManager config("test", move(stubJFS));
    ASSERT_TRUE(config.load());
}

TEST(ConfigurationManager_save, writeFileIsFalse_ReturnFalse) 
{
    const string lastErrorSample { "This is an error" };
    
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, getLastError()).WillByDefault(ReturnRef(lastErrorSample));
    ON_CALL(*stubJFS, writeFile(_)).WillByDefault(Return(false));
    ConfigurationManager config("test", move(stubJFS));
    ASSERT_FALSE(config.save());
    ASSERT_EQ("This is an error", config.getLastError());
}

TEST(ConfigurationManager_save, writeFileIsTrue_ReturnTrue) 
{
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, writeFile(_)).WillByDefault(Return(true));
    ConfigurationManager config("test", move(stubJFS));
    ASSERT_TRUE(config.save());
}

TEST(ConfigurationManager_fileExists, fileExistsIsTrue_ReturnTrue) 
{
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(true));
    ConfigurationManager config("test", move(stubJFS));
    ASSERT_TRUE(config.fileExists());
}

TEST(ConfigurationManager_fileExists, fileExistsIsFalse_ReturnFalse) 
{
    auto stubJFS = make_unique<NiceMock<StubJSONFileStream>>("test");
    ON_CALL(*stubJFS, fileExists()).WillByDefault(Return(false));
    ConfigurationManager config("test", move(stubJFS));
    ASSERT_FALSE(config.fileExists());
}
