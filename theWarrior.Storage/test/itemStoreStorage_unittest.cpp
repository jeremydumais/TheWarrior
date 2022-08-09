#include "itemStoreStorage.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace thewarrior::models;
using namespace thewarrior::storage;
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

class ItemStoreStorageEmptyISStubFS : public ::testing::Test
{
public:
    ItemStoreStorageEmptyISStubFS()
    {
        stubBFS = make_unique<NiceMock<StubBinaryFileStream<ItemStore>>>();
        itemStore = make_shared<ItemStore>();
        ON_CALL(*stubBFS, open(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, write(_)).WillByDefault(Return(true));
        ON_CALL(*stubBFS, close()).WillByDefault(Return(true));
    }
    unique_ptr<NiceMock<StubBinaryFileStream<ItemStore>>> stubBFS;
    shared_ptr<ItemStore> itemStore;
    ItemStoreStorage itemStoreStorage;
};

TEST(ItemStoreStorage_loadItemStore, WithEmptyFileName_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.loadItemStore("", shared_ptr<ItemStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(ItemStoreStorage_loadItemStore, WithWhiteSpacesFileName_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.loadItemStore("  ", shared_ptr<ItemStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(ItemStoreStorage_loadItemStore, WithNullItemStore_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.loadItemStore("store.db", nullptr);
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The itemStore cannot be null.", err.what());
    }
}

TEST_F(ItemStoreStorageEmptyISStubFS, loadItemStore_FileStreamFailToOpen_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.loadItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to open the itemStore test", err.what());
	}
}

TEST_F(ItemStoreStorageEmptyISStubFS, loadItemStore_FileStreamFailToReadAllInto_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, readAllInto(_)).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.loadItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to read the content of the itemStore test", err.what());
	}
}

TEST_F(ItemStoreStorageEmptyISStubFS, loadItemStore_FileStreamFailToClose_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.loadItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to close the itemStore file test", err.what());
	}
}

TEST_F(ItemStoreStorageEmptyISStubFS, loadItemStore_FileStreamSucceedToOpen_ReturnSuccess)
{
    itemStoreStorage.setFileStream(move(stubBFS));
    itemStoreStorage.loadItemStore("test", itemStore);
}

TEST(ItemStoreStorage_saveItemStore, WithEmptyFileName_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.saveItemStore("", shared_ptr<ItemStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(ItemStoreStorage_saveItemStore, WithWhiteSpacesFileName_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.saveItemStore("  ", shared_ptr<ItemStore>());
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The fileName cannot be empty.", err.what());
    }
}

TEST(ItemStoreStorage_saveItemStore, WithNullItemStore_ThrowInvalidArgument)
{
    try {
        ItemStoreStorage itemStoreStorage;
        itemStoreStorage.saveItemStore("store.db", nullptr);
        FAIL();
    }
    catch (const invalid_argument &err) {
        ASSERT_STREQ("The itemStore cannot be null.", err.what());
    }
}

TEST_F(ItemStoreStorageEmptyISStubFS, saveItemStore_FileStreamFailToOpen_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, open(_)).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.saveItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to open the itemStore test", err.what());
	}
}

TEST_F(ItemStoreStorageEmptyISStubFS, saveItemStore_FileStreamFailToWrite_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, write(_)).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.saveItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to write the content of the itemStore test", err.what());
	}
}


TEST_F(ItemStoreStorageEmptyISStubFS, saveItemStore_FileStreamFailToClose_ThrowRuntimeError)
{
    ON_CALL(*stubBFS, close()).WillByDefault(Return(false));

    itemStoreStorage.setFileStream(move(stubBFS));
    try {
        itemStoreStorage.saveItemStore("test", itemStore);
        FAIL();
    }
    catch(runtime_error &err) {
        ASSERT_STREQ("Unable to close the itemStore file test", err.what());
	}
}

TEST_F(ItemStoreStorageEmptyISStubFS, saveItemStore_FileStreamSucceedToSave_ReturnSuccess)
{
    itemStoreStorage.saveItemStore("test", itemStore);
}
