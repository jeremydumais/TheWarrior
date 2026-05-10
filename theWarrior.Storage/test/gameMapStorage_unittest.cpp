#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <utility>
#include "gameMapStorage.hpp"
#include "iBinaryFileStream.hpp"

using namespace thewarrior::models;
using namespace thewarrior::storage;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::_;

template<class T>
class StubBinaryFileStream : public IBinaryFileStream<T> {
 public:
    StubBinaryFileStream() : IBinaryFileStream<T>("") {}
    MOCK_METHOD(bool, open, (FileOpenMode), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, readAllInto, (T&), (override));
    MOCK_METHOD(bool, write, (const T&), (override));
    MOCK_METHOD(bool, remove, (), (override));
};

class GameMapStorageSampleMapStubFS : public ::testing::Test {
 public:
    GameMapStorageSampleMapStubFS()
    : map(std::make_shared<GameMap>(1, 1)),
    stubBFS(std::make_unique<NiceMock<StubBinaryFileStream<GameMap>>>()) {
        ON_CALL(*stubBFS, open(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, write(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));
    }
    ~GameMapStorageSampleMapStubFS() override;
    std::shared_ptr<GameMap> map;
    std::unique_ptr<NiceMock<StubBinaryFileStream<GameMap>>> stubBFS;
    GameMapStorage mapStorage;
};

GameMapStorageSampleMapStubFS::~GameMapStorageSampleMapStubFS() {}

TEST(GameMapStorage_loadMap, fileNameEmpty_ThrowInvalidArgument) {
    try {
        std::shared_ptr<GameMap> map = std::make_shared<GameMap>(1, 1);
        GameMapStorage mapStorage;
        mapStorage.loadMap("", map);
        FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
    }
}

TEST(GameMapStorage_loadMap, fileNameWhiteSpaces_ThrowInvalidArgument) {
    try {
        std::shared_ptr<GameMap> map = std::make_shared<GameMap>(1, 1);
        GameMapStorage mapStorage;
        mapStorage.loadMap("   ", map);
        FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("The filename cannot be empty!", err.what());
    }
}

TEST(GameMapStorage_loadMap, mapNullPtr_ThrowInvalidArgument) {
    try {
        std::shared_ptr<GameMap> map;
        GameMapStorage mapStorage;
        mapStorage.loadMap("test", map);
        FAIL();
    } catch(const std::invalid_argument &err) {
        ASSERT_STREQ("The map cannot be null!", err.what());
    }
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToOpen_ThrowRuntimeError) {
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    mapStorage.setFileStream(std::move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    } catch(const std::runtime_error &err) {
        ASSERT_STREQ("Unable to open the map test", err.what());
    }
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToReadAllInto_ThrowRuntimeError) {
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    mapStorage.setFileStream(std::move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    } catch(const std::runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the map test", err.what());
    }
}

TEST_F(GameMapStorageSampleMapStubFS, loadMap_FileStreamFailToClose_ThrowRuntimeError) {
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    mapStorage.setFileStream(std::move(stubBFS));
    try {
        mapStorage.loadMap("test", map);
        FAIL();
    } catch(const std::runtime_error &err) {
        ASSERT_STREQ("Unable to close the map file test", err.what());
    }
}
