#include "monsterStoreStorage.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

template <class T>
class StubBinaryFileStream : public IBinaryFileStream<T>
{
public:
    StubBinaryFileStream() : IBinaryFileStream<T>("") {}
    MOCK_METHOD(bool, open, (FileOpenMode), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, readAllInto, (T &), (override));
    MOCK_METHOD(bool, write, (const T&), (override));
};

class MonsterStoreStorageEmptyISStubFS : public ::testing::Test
{
public:
    MonsterStoreStorageEmptyISStubFS()
    {
        stubBFS = make_unique<NiceMock<StubBinaryFileStream<MonsterStore>>>();
        monsterStore = make_shared<MonsterStore>();
        ON_CALL(*stubBFS, open(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, write(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));
    }
    unique_ptr<NiceMock<StubBinaryFileStream<MonsterStore>>> stubBFS;
    shared_ptr<MonsterStore> monsterStore;
    MonsterStoreStorage monsterStoreStorage;
};

TEST(MonsterStoreStorage_loadMonsterStore, WithEmptyFileName_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.loadMonsterStore("", shared_ptr<MonsterStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(MonsterStoreStorage_loadMonsterStore, WithWhiteSpacesFileName_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.loadMonsterStore("  ", shared_ptr<MonsterStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(MonsterStoreStorage_loadMonsterStore, WithNullMonsterStore_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.loadMonsterStore("store.db", nullptr);
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The monsterStore cannot be null.", err.what());
    }
}

TEST_F(MonsterStoreStorageEmptyISStubFS, loadMonsterStore_FileStreamFailToOpen_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.loadMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to open the monsterStore test", err.what());
	}
}

TEST_F(MonsterStoreStorageEmptyISStubFS, loadMonsterStore_FileStreamFailToReadAllInto_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.loadMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the monsterStore test", err.what());
	}
}

TEST_F(MonsterStoreStorageEmptyISStubFS, loadMonsterStore_FileStreamFailToClose_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.loadMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to close the monsterStore file test", err.what());
	}
}

TEST_F(MonsterStoreStorageEmptyISStubFS, loadMonsterStore_FileStreamSucceedToOpen_ReturnSuccess)
{
    monsterStoreStorage.setFileStream(move(stubBFS));
    monsterStoreStorage.loadMonsterStore("test", monsterStore);
}

TEST(MonsterStoreStorage_saveMonsterStore, WithEmptyFileName_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.saveMonsterStore("", shared_ptr<MonsterStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(MonsterStoreStorage_saveMonsterStore, WithWhiteSpacesFileName_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.saveMonsterStore("  ", shared_ptr<MonsterStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(MonsterStoreStorage_saveMonsterStore, WithNullMonsterStore_ThrowInvalidArgument)
{
    try {
        MonsterStoreStorage monsterStoreStorage;
        monsterStoreStorage.saveMonsterStore("store.db", nullptr);
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The monsterStore cannot be null.", err.what());
    }
}

TEST_F(MonsterStoreStorageEmptyISStubFS, saveMonsterStore_FileStreamFailToOpen_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.saveMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to open the monsterStore test", err.what());
	}
}

TEST_F(MonsterStoreStorageEmptyISStubFS, saveMonsterStore_FileStreamFailToWrite_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, write(_)).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.saveMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to write the content of the monsterStore test", err.what());
	}
}


TEST_F(MonsterStoreStorageEmptyISStubFS, saveMonsterStore_FileStreamFailToClose_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    monsterStoreStorage.setFileStream(move(stubBFS));
    try {
        monsterStoreStorage.saveMonsterStore("test", monsterStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to close the monsterStore file test", err.what());
	}
}

TEST_F(MonsterStoreStorageEmptyISStubFS, saveMonsterStore_FileStreamSucceedToSave_ReturnSuccess)
{
    monsterStoreStorage.saveMonsterStore("test", monsterStore);
}