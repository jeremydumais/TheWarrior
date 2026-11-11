#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "gameDataStorage.hpp"
#include "armorItem.hpp"
#include "statsItem.hpp"
#include "weaponItem.hpp"
#include <filesystem>
#include <fstream>
#include <random>

namespace {

using namespace thewarrior::models;
using namespace thewarrior::storage;
using ::testing::_;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;

template <class T>
class GameDataStubBinaryFileStream : public IBinaryFileStream<T> {
 public:
    GameDataStubBinaryFileStream() : IBinaryFileStream<T>("") {}
    MOCK_METHOD(bool, open, (FileOpenMode), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, readAllInto, (T &), (override));
    MOCK_METHOD(bool, write, (const T&), (override));
    MOCK_METHOD(bool, remove, (), (override));
};

class GameDataStorageEmptyGDStubFS : public ::testing::Test {
 public:
    GameDataStorageEmptyGDStubFS() {
        stubBFS = std::make_unique<NiceMock<GameDataStubBinaryFileStream<GameData>>>();
        gameData = std::make_shared<GameData>();
        ON_CALL(*stubBFS, open(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, write(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));
    }
    std::unique_ptr<NiceMock<GameDataStubBinaryFileStream<GameData>>> stubBFS;
    std::shared_ptr<GameData> gameData;
    GameDataStorage gameDataStorage;
};

TEST(GameDataStorage_loadGameData, WithEmptyFileName_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.loadGameData("", std::make_shared<GameData>());
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(GameDataStorage_loadGameData, WithWhiteSpacesFileName_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.loadGameData(" \t\n ", std::make_shared<GameData>());
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(GameDataStorage_loadGameData, WithNullGameData_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.loadGameData("store.db", nullptr);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The gameData cannot be null.", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, loadGameData_FileStreamFailToOpen_ThrowRuntimeError) {
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.loadGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to open the gameData test", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, loadGameData_FileStreamFailToReadAllInto_ThrowRuntimeError) {
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.loadGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the gameData test", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, loadGameData_FileStreamFailToClose_ThrowRuntimeError) {
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.loadGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to close the gameData file test", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, loadGameData_FileStreamSucceedToOpen_ReturnSuccess) {
    InSequence sequence;
    EXPECT_CALL(*stubBFS, open(FileOpenMode::Read)).WillOnce(Return(true));
    EXPECT_CALL(*stubBFS, readAllInto(Ref(*gameData))).WillOnce(Return(true));
    EXPECT_CALL(*stubBFS, close()).WillOnce(Return(true));

    gameDataStorage.setFileStream(std::move(stubBFS));
    EXPECT_NO_THROW(gameDataStorage.loadGameData("test", gameData));
}

TEST(GameDataStorage_saveGameData, WithEmptyFileName_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.saveGameData("", std::make_shared<GameData>());
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(GameDataStorage_saveGameData, WithWhiteSpacesFileName_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.saveGameData(" \t\n ", std::make_shared<GameData>());
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(GameDataStorage_saveGameData, WithNullGameData_ThrowInvalidArgument) {
    try {
        GameDataStorage gameDataStorage;
        gameDataStorage.saveGameData("store.db", nullptr);
        FAIL();
    }
    catch (const std::invalid_argument &err) {
        ASSERT_STREQ("The gameData cannot be null.", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, saveGameData_FileStreamFailToOpen_ThrowRuntimeError) {
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.saveGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to open the gameData test", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, saveGameData_FileStreamFailToWrite_ThrowRuntimeError) {
    ON_CALL(*stubBFS, write(_)).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.saveGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to write the content of the gameData test", err.what());
    }
}


TEST_F(GameDataStorageEmptyGDStubFS, saveGameData_FileStreamFailToClose_ThrowRuntimeError) {
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    gameDataStorage.setFileStream(std::move(stubBFS));
    try {
        gameDataStorage.saveGameData("test", gameData);
        FAIL();
    }
    catch(std::runtime_error &err) {
        ASSERT_STREQ("Unable to close the gameData file test", err.what());
    }
}

TEST_F(GameDataStorageEmptyGDStubFS, saveGameData_FileStreamSucceedToSave_ReturnSuccess) {
    InSequence sequence;
    EXPECT_CALL(*stubBFS, open(FileOpenMode::Write)).WillOnce(Return(true));
    EXPECT_CALL(*stubBFS, write(Ref(*gameData))).WillOnce(Return(true));
    EXPECT_CALL(*stubBFS, close()).WillOnce(Return(true));

    gameDataStorage.setFileStream(std::move(stubBFS));
    EXPECT_NO_THROW(gameDataStorage.saveGameData("test", gameData));
}


class GameDataStorageFiles : public ::testing::Test {
protected:
    void SetUp() override {
        directory = std::filesystem::temp_directory_path() /
            ("warrior-gamedata-test-" + std::to_string(std::random_device{}()));
        ASSERT_TRUE(std::filesystem::create_directory(directory));
        fileName = (directory / "gameData.gdt").string();
    }
    void TearDown() override {
        std::error_code error;
        std::filesystem::remove_all(directory, error);
    }
    std::filesystem::path directory;
    std::string fileName;
};

TEST_F(GameDataStorageFiles, EmptyGameDataRoundTrip) {
    auto source = std::make_shared<GameData>();
    GameDataStorage writer;
    ASSERT_NO_THROW(writer.saveGameData(fileName, source));
    auto loaded = std::make_shared<GameData>();
    GameDataStorage reader;
    ASSERT_NO_THROW(reader.loadGameData(fileName, loaded));
    EXPECT_EQ(0, loaded->getItemStore().getItemCount());
    EXPECT_EQ(0, loaded->getItemStore().getTextureContainer().getCount());
}

TEST_F(GameDataStorageFiles, RoundTripPreservesAllItemTypesAndTextures) {
    auto source = std::make_shared<GameData>();
    auto store = source->getItemStoreForEdition();
    ASSERT_TRUE(store->getTextureContainerForEdition().addTexture(
        TextureInfo{"Tex1", "items.png", 64, 32, 16, 16}));
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0, "Description", 10, 5})));
    ASSERT_TRUE(store->addItem(std::make_shared<WeaponItem>(
        WeaponItemCreationInfo{"swd001", "Sword1", "Tex1", 1, "Sword", 20, 10,
                               2.5F, WeaponBodyPart::SecondaryHand})));
    ASSERT_TRUE(store->addItem(std::make_shared<ArmorItem>(
        ArmorItemCreationInfo{"arm001", "Armor1", "Tex1", 2, "Armor", 30, 15,
                              3.5F, ArmorBodyPart::Hands})));
    ASSERT_TRUE(store->addItem(std::make_shared<StatsItem>(
        StatsItemCreationInfo{"pot001", "Potion1", "Tex1", 3, "Potion", 40, 20,
                              Stats::Endurance, 4.5F, true, 3})));
    GameDataStorage writer;
    ASSERT_NO_THROW(writer.saveGameData(fileName, source));

    auto loaded = std::make_shared<GameData>();
    auto editorStore = loaded->getItemStoreForEdition();
    ASSERT_TRUE(editorStore->addItem(std::make_shared<Item>(
        ItemCreationInfo{"old001", "Old item", "Tex1", 0})));
    GameDataStorage reader;
    ASSERT_NO_THROW(reader.loadGameData(fileName, loaded));

    EXPECT_EQ(editorStore, loaded->getItemStoreForEdition());
    EXPECT_EQ(nullptr, editorStore->findItem("old001"));
    ASSERT_EQ(4, editorStore->getItemCount());
    for (const auto &original : store->getItems()) {
        auto restored = editorStore->findItem(original->getId());
        ASSERT_NE(nullptr, restored);
        EXPECT_EQ(original->getType(), restored->getType());
        EXPECT_TRUE(*original == *restored) << original->getId();
    }
    auto texture = editorStore->getTextureContainer().getTextureByName("Tex1");
    ASSERT_TRUE(texture.has_value());
    EXPECT_EQ("items.png", texture->get().getFilename());
    EXPECT_EQ(64, texture->get().getWidth());
    EXPECT_EQ(32, texture->get().getHeight());
    EXPECT_EQ(16, texture->get().getTileWidth());
    EXPECT_EQ(16, texture->get().getTileHeight());
}

TEST_F(GameDataStorageFiles, MissingFileThrowsWithoutCreatingIt) {
    GameDataStorage storage;
    EXPECT_THROW(storage.loadGameData(fileName, std::make_shared<GameData>()),
                 std::runtime_error);
    EXPECT_FALSE(std::filesystem::exists(fileName));
}

TEST_F(GameDataStorageFiles, CorruptFileThrows) {
    {
        std::ofstream file(fileName, std::ios::binary);
        file << "invalid game data";
    }
    GameDataStorage storage;
    EXPECT_THROW(storage.loadGameData(fileName, std::make_shared<GameData>()),
                 std::runtime_error);
}

TEST_F(GameDataStorageFiles, SaveWithMissingParentThrows) {
    GameDataStorage storage;
    EXPECT_THROW(storage.saveGameData((directory / "missing" / "gameData.gdt").string(),
                                     std::make_shared<GameData>()), std::runtime_error);
}

}  // namespace
