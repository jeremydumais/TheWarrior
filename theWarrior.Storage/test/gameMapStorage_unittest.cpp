#include "gameMapStorage.hpp"
#include "iBinaryFileStream.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::_;

template<class T>
class StubBinaryFileStream : public IBinaryFileStream<T>
{
public:
    StubBinaryFileStream() : IBinaryFileStream<T>("") {}
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, readAllInto, (T&), (override));
};

class GameMapStorageSampleMapStubFS : public ::testing::Test
{
public:
    GameMapStorageSampleMapStubFS() {
        stubBFS = make_unique<NiceMock<StubBinaryFileStream<GameMap>>>();
        map = make_shared<GameMap>(1, 1);
        ON_CALL(*stubBFS, open()).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));

    }
    unique_ptr<NiceMock<StubBinaryFileStream<GameMap>>> stubBFS;
    shared_ptr<GameMap> map;
    GameMapStorage mapStorage;
};

TEST(GameMapStorage_loadMap, fileNameEmpty_ThrowInvalidArgument)
{
    try {
        shared_ptr<GameMap> map = make_shared<GameMap>(1, 1);
        GameMapStorage mapStorage;
        mapStorage.loadMap("", map);
		FAIL();
    }
    catch(invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
	}
}

TEST(GameMapStorage_loadMap, fileNameWhiteSpaces_ThrowInvalidArgument)
{
    try {
        shared_ptr<GameMap> map = make_shared<GameMap>(1, 1);
        GameMapStorage mapStorage;
        mapStorage.loadMap("   ", map);
		FAIL();
    }
    catch(invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
	}
}

TEST(GameMapStorage_loadMap, mapNullPtr_ThrowInvalidArgument)
{
    try {
        shared_ptr<GameMap> map;
        GameMapStorage mapStorage;
        mapStorage.loadMap("test", map);
		FAIL();
    }
    catch(invalid_argument &err) {
        ASSERT_STREQ("The map cannot be null!", err.what());
	}
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToOpen_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, open()).WillByDefault(Return(false));

    mapStorage.setFileStream(move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to open the map test", err.what());
	}
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToReadAllInto_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    mapStorage.setFileStream(move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the map test", err.what());
	}
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToClose_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    mapStorage.setFileStream(move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to close the map file test", err.what());
	}
}