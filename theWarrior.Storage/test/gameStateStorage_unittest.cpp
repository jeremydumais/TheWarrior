#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <utility>
#include "gameState.hpp"
#include "gameStateStorage.hpp"
#include "player.hpp"

using namespace thewarrior::models;
using namespace thewarrior::storage;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

template <class T>
class StubBinaryFileStream : public IBinaryFileStream<T> {
 public:
    StubBinaryFileStream() : IBinaryFileStream<T>("") {}
    MOCK_METHOD(bool, open, (FileOpenMode), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, readAllInto, (T &), (override));
    MOCK_METHOD(bool, write, (const T&), (override));
};

class GameStateStorageDefault : public ::testing::Test {
 public:
    GameStateStorageDefault()
    : player("Test"), gameState(player, worldState) {}
    Player player;
    WorldState worldState;
    GameState gameState;
    GameStateStorage gameStateStorage;
};

class GameStateStorageEmptyISStubFS : public ::testing::Test {
 public:
    GameStateStorageEmptyISStubFS()
    : stubBFS(std::make_unique<NiceMock<StubBinaryFileStream<GameState>>>()),
    player("Test"),
    gameState(player, worldState) {
        ON_CALL(*stubBFS, open(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, write(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));
    }
    std::unique_ptr<NiceMock<StubBinaryFileStream<GameState>>> stubBFS;
    Player player;
    WorldState worldState;
    GameState gameState;
    GameStateStorage gameStateStorage;
};

TEST_F(GameStateStorageDefault, loadGameState_WithEmptyFileName_ThrowInvalidArgument) {
    try {
        gameStateStorage.loadGameState("", gameState);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST_F(GameStateStorageDefault, loadGameState_WithWhiteSpacesFileName_ThrowInvalidArgument) {
    try {
        gameStateStorage.loadGameState("  ", gameState);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, loadGameState_FileStreamFailToOpen_ThrowRuntimeError) {
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.loadGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to open the gameState test", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, loadGameState_FileStreamFailToReadAllInto_ThrowRuntimeError) {
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.loadGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the gameState test", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, loadGameState_FileStreamFailToClose_ThrowRuntimeError) {
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.loadGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to close the gameState file test", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, loadGameState_FileStreamSucceedToOpen_ReturnSuccess) {
    gameStateStorage.setFileStream(std::move(stubBFS));
    gameStateStorage.loadGameState("test", gameState);
}

TEST_F(GameStateStorageDefault, saveGameState_WithEmptyFileName_ThrowInvalidArgument) {
    try {
        gameStateStorage.saveGameState("", gameState);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST_F(GameStateStorageDefault, saveGameState_WithWhiteSpacesFileName_ThrowInvalidArgument) {
    try {
        gameStateStorage.saveGameState("  ", gameState);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, saveGameState_FileStreamFailToOpen_ThrowRuntimeError) {
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.saveGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to open the gameState test", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, saveGameState_FileStreamFailToWrite_ThrowRuntimeError) {
    ON_CALL(*stubBFS, write(_)).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.saveGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to write the content of the gameState test", err.what());
    }
}


TEST_F(GameStateStorageEmptyISStubFS, saveGameState_FileStreamFailToClose_ThrowRuntimeError) {
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    gameStateStorage.setFileStream(std::move(stubBFS));
    try {
        gameStateStorage.saveGameState("test", gameState);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to close the gameState file test", err.what());
    }
}

TEST_F(GameStateStorageEmptyISStubFS, saveGameState_FileStreamSucceedToSave_ReturnSuccess) {
    gameStateStorage.setFileStream(std::move(stubBFS));
    gameStateStorage.saveGameState("test", gameState);
}
