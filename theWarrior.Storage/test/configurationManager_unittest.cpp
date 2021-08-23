#include "configurationManager.hpp"
#include "iJSONFileStream.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::_;

class StubJSONFileStream : public IJSONFileStream
{
public:
    StubJSONFileStream() : IJSONFileStream("") {}
    MOCK_METHOD(bool, fileExists, (), (const override));
    MOCK_METHOD(bool, readFile, (boost::property_tree::ptree &obj), (override));
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