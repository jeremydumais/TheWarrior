#include "mainController.hpp"
#include "armorItem.hpp"
#include "statsItem.hpp"
#include "weaponItem.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <filesystem>
#include <fstream>
#include <QTemporaryDir>
#include <QGuiApplication>
#include <QImage>
#include "gameDataStorage.hpp"
#include "itemStoreStorage.hpp"
#include "texturePickerController.hpp"

using namespace gamedataeditor::controllers;
using namespace thewarrior::models;

class MainControllerLoadedGameData : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(resources.isValid());
        controller.setResourcesPath(resources.path().toStdString());
        ASSERT_TRUE(controller.loadOrCreateGameData()) << controller.getLastError();
    }
    QTemporaryDir resources;
    MainController controller;
};

class MainControllerSampleWithOneItem : public MainControllerLoadedGameData
{
public:
    void SetUp() override
    {
        ASSERT_NO_FATAL_FAILURE(MainControllerLoadedGameData::SetUp());
        auto itemStore = controller.getItemStoreForEdition();
        itemStore->addItem(std::make_shared<Item>(ItemCreationInfo { "ite001", "Item1", "Tex1", 0}));
    }
};

class MainControllerSampleWithOneItemPerCategory : public MainControllerLoadedGameData
{
public:
    void SetUp() override
    {
        ASSERT_NO_FATAL_FAILURE(MainControllerLoadedGameData::SetUp());
        auto itemStore = controller.getItemStoreForEdition();
        itemStore->addItem(std::make_shared<Item>(ItemCreationInfo { "ite001", "Item1", "Tex1", 0}));
        itemStore->addItem(std::make_shared<WeaponItem>(WeaponItemCreationInfo { "swd001", "Sword1", "Tex2", 1, "A sword", 2, 1, 2.1F, WeaponBodyPart::SecondaryHand}));
        itemStore->addItem(std::make_shared<ArmorItem>(ArmorItemCreationInfo { "arm001", "Armor1", "Tex3", 2, "", 2, 1, 3.2F, ArmorBodyPart::Hands}));
        itemStore->addItem(std::make_shared<StatsItem>(StatsItemCreationInfo { "pot001", "Potion1", "Tex4", 3, "", 2, 1, Stats::Endurance, 4.1F, true, 0}));
    }
};

TEST_F(MainControllerLoadedGameData, getItemCategories_WithEmptyItemStore_ReturnEmpty)
{
    ASSERT_EQ(0, controller.getItemCategories().size());
}

TEST_F(MainControllerLoadedGameData, setItemStoreTexture_PickerUsesUpdatedTexture)
{
    TexturePickerController picker(controller.getItemStore().getTextureContainer());
    auto texture = std::make_unique<commoneditor::ui::TextureDTO>();
    texture->name = "ItemsTile";
    texture->filename = "original-items-texture.png";
    texture->width = 512;
    texture->height = 256;
    texture->tileWidth = 32;
    texture->tileHeight = 16;
    ASSERT_TRUE(controller.setItemStoreTexture(std::move(texture))) << controller.getLastError();
    ASSERT_EQ(std::vector<std::string>{"ItemsTile"}, picker.getTextureNames());
    ASSERT_EQ("/resources/textures/original-items-texture.png",
              picker.getTextureFileName("/resources", "ItemsTile"));

    auto replacement = std::make_unique<commoneditor::ui::TextureDTO>();
    replacement->name = "ItemsTile";
    replacement->filename = "replacement-items-texture.png";
    replacement->width = 256;
    replacement->height = 128;
    replacement->tileWidth = 16;
    replacement->tileHeight = 16;
    ASSERT_TRUE(controller.setItemStoreTexture(std::move(replacement))) << controller.getLastError();
    ASSERT_EQ(std::vector<std::string>{"ItemsTile"}, picker.getTextureNames());
    ASSERT_EQ("/resources/textures/replacement-items-texture.png",
              picker.getTextureFileName("/resources", "ItemsTile"));
    ASSERT_EQ(1, picker.getTextureIndexFromPosition(Point(17, 127), "ItemsTile"));
}

TEST_F(MainControllerSampleWithOneItem, getItemCategories_ReturnOneCategory)
{
    auto itemCategories = controller.getItemCategories();
    ASSERT_EQ(1, itemCategories.size());
    ASSERT_TRUE(itemCategories.find("Item") != itemCategories.end());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemCategories_ReturnFourCategories)
{
    auto itemCategories = controller.getItemCategories();
    ASSERT_EQ(4, itemCategories.size());
    ASSERT_TRUE(itemCategories.find("Item") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Weapon") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Armor") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Stats Item") != itemCategories.end());
}

TEST_F(MainControllerSampleWithOneItem, getItemsFromCategory_WithEmptyCategory_ReturnEmpty)
{
    auto items = controller.getItemsFromCategory("");
    ASSERT_EQ(0, items.size());
}

TEST_F(MainControllerSampleWithOneItem, getItemsFromCategory_WithItem_Return1Item)
{
    auto items = controller.getItemsFromCategory("Item");
    ASSERT_EQ(1, items.size());
    ASSERT_EQ("ite001", items[0].id);
    ASSERT_EQ("Item1", items[0].name);
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemsFromCategory_WithStatsItem_Return1Item)
{
    auto items = controller.getItemsFromCategory("Stats Item");
    ASSERT_EQ(1, items.size());
    ASSERT_EQ("pot001", items[0].id);
    ASSERT_EQ("Potion1", items[0].name);
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithNotExistantId_ReturnNullOpt)
{
    ASSERT_FALSE(controller.getItemTypeFromItemId("ite000").has_value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithIte001_ReturnItem)
{
    auto actual = controller.getItemTypeFromItemId("ite001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Item, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithSwd001_ReturnWeapon)
{
    auto actual = controller.getItemTypeFromItemId("swd001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Weapon, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithArm001_ReturnArmor)
{
    auto actual = controller.getItemTypeFromItemId("arm001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Armor, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithPot001_ReturnPotion)
{
    auto actual = controller.getItemTypeFromItemId("pot001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::StatsItem, actual.value());
}


class MainControllerPersistence : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_TRUE(resources.isValid());
        resourcesPath = resources.path().toStdString();
        gameDataPath = std::filesystem::path(resourcesPath) / "gamedata" / "gameData.gdt";
        controller.setResourcesPath(resourcesPath);
    }
    QTemporaryDir resources;
    std::string resourcesPath;
    std::filesystem::path gameDataPath;
    MainController controller;
};

TEST_F(MainControllerPersistence, CreatesDirectoriesAndReadableEmptyGameData) {
    EXPECT_TRUE(controller.getLastError().empty());
    ASSERT_TRUE(controller.loadOrCreateGameData()) << controller.getLastError();
    EXPECT_TRUE(std::filesystem::is_regular_file(gameDataPath));
    EXPECT_EQ(0, controller.getItemStore().getItemCount());
    auto data = std::make_shared<GameData>();
    thewarrior::storage::GameDataStorage storage;
    ASSERT_NO_THROW(storage.loadGameData(gameDataPath.string(), data));
    EXPECT_EQ(0, data->getItemStore().getItemCount());
}

TEST_F(MainControllerPersistence, LoadsExistingGameDataInsteadOfLegacyStore) {
    std::filesystem::create_directories(gameDataPath.parent_path());
    auto data = std::make_shared<GameData>();
    ASSERT_TRUE(data->getItemStoreForEdition()->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Saved item", "Tex1", 0})));
    thewarrior::storage::GameDataStorage storage;
    storage.saveGameData(gameDataPath.string(), data);
    std::filesystem::create_directories(std::filesystem::path(resourcesPath) / "items");
    auto legacy = std::make_shared<ItemStore>();
    ASSERT_TRUE(legacy->addItem(std::make_shared<Item>(
        ItemCreationInfo{"old001", "Legacy item", "Tex1", 0})));
    thewarrior::storage::ItemStoreStorage legacyStorage;
    legacyStorage.saveItemStore(resourcesPath + "/items/itemstore.itm", legacy);

    ASSERT_TRUE(controller.loadOrCreateGameData()) << controller.getLastError();
    ASSERT_EQ(1, controller.getItemStore().getItemCount());
    ASSERT_NE(nullptr, controller.getItemStore().findItem("ite001"));
    EXPECT_EQ("Saved item", controller.getItemStore().findItem("ite001")->getName());
    EXPECT_EQ(nullptr, controller.getItemStore().findItem("old001"));
}

TEST_F(MainControllerPersistence, MigratesLegacyItemStoreAndPersistsIt) {
    std::filesystem::create_directories(std::filesystem::path(resourcesPath) / "items");
    auto legacy = std::make_shared<ItemStore>();
    ASSERT_TRUE(legacy->addItem(std::make_shared<WeaponItem>(
        WeaponItemCreationInfo{"swd001", "Sword1", "Tex1", 0, "", 10, 5,
                               2.5F, WeaponBodyPart::MainHand})));
    ASSERT_TRUE(legacy->getTextureContainerForEdition().addTexture(
        TextureInfo{"Tex1", "items.png", 32, 32, 16, 16}));
    thewarrior::storage::ItemStoreStorage legacyStorage;
    legacyStorage.saveItemStore(resourcesPath + "/items/itemstore.itm", legacy);

    ASSERT_TRUE(controller.loadOrCreateGameData()) << controller.getLastError();
    EXPECT_EQ(1, controller.getItemStore().getItemCount());
    EXPECT_EQ(1, controller.getItemStore().getTextureContainer().getCount());
    auto saved = std::make_shared<GameData>();
    thewarrior::storage::GameDataStorage storage;
    storage.loadGameData(gameDataPath.string(), saved);
    auto item = saved->getItemStore().findItem("swd001");
    ASSERT_NE(nullptr, item);
    EXPECT_EQ(ItemType::Weapon, item->getType());
    EXPECT_TRUE(*legacy->findItem("swd001") == *item);
    EXPECT_EQ(1, saved->getItemStore().getTextureContainer().getCount());
}

TEST_F(MainControllerPersistence, CorruptGameDataReturnsFalseAndReportsError) {
    std::filesystem::create_directories(gameDataPath.parent_path());
    {
        std::ofstream file(gameDataPath, std::ios::binary);
        file << "invalid game data";
    }
    EXPECT_FALSE(controller.loadOrCreateGameData());
    EXPECT_EQ("Unable to read the content of the gameData " + gameDataPath.string(),
              controller.getLastError());
}

TEST_F(MainControllerPersistence, CorruptLegacyStoreReturnsFalseWithoutSavingGameData) {
    std::filesystem::create_directories(std::filesystem::path(resourcesPath) / "items");
    const auto legacyPath = resourcesPath + "/items/itemstore.itm";
    {
        std::ofstream file(legacyPath, std::ios::binary);
        file << "invalid item store";
    }
    EXPECT_FALSE(controller.loadOrCreateGameData());
    EXPECT_EQ("Unable to read the content of the itemStore " + legacyPath,
              controller.getLastError());
    EXPECT_FALSE(std::filesystem::exists(gameDataPath));
}

TEST_F(MainControllerPersistence, ResourcesPathThatIsAFileReturnsFalseAndReportsError) {
    const auto blockedPath = resourcesPath + "/blocked";
    { std::ofstream file(blockedPath); }
    controller.setResourcesPath(blockedPath);
    EXPECT_FALSE(controller.loadOrCreateGameData());
    EXPECT_FALSE(controller.getLastError().empty());
}

TEST_F(MainControllerLoadedGameData, EditingAccessorUpdatesReadableStore) {
    auto store = controller.getItemStoreForEdition();
    ASSERT_NE(nullptr, store);
    EXPECT_EQ(store.get(), &controller.getItemStore());
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0})));
    EXPECT_NE(nullptr, controller.getItemStore().findItem("ite001"));
}

TEST_F(MainControllerSampleWithOneItem, DeleteExistingItemUpdatesSharedStore) {
    auto store = controller.getItemStoreForEdition();
    ASSERT_TRUE(controller.deleteItem("ite001"));
    EXPECT_EQ(nullptr, store->findItem("ite001"));
    EXPECT_EQ(0, controller.getItemStore().getItemCount());
    EXPECT_TRUE(controller.getItemCategories().empty());
}

TEST_F(MainControllerSampleWithOneItem, DeleteMissingItemReportsErrorAndKeepsExistingItem) {
    EXPECT_FALSE(controller.deleteItem("missing"));
    EXPECT_EQ("Unable to delete the item missing", controller.getLastError());
    EXPECT_NE(nullptr, controller.getItemStore().findItem("ite001"));
}

TEST_F(MainControllerLoadedGameData, CategoryNameMapsToItemType) {
    EXPECT_EQ(ItemType::Item, controller.getItemTypeFromCategoryName("Item"));
    EXPECT_EQ(ItemType::Weapon, controller.getItemTypeFromCategoryName("Weapon"));
    EXPECT_EQ(ItemType::Armor, controller.getItemTypeFromCategoryName("Armor"));
    EXPECT_EQ(ItemType::StatsItem, controller.getItemTypeFromCategoryName("Stats Item"));
    EXPECT_FALSE(controller.getItemTypeFromCategoryName("unknown").has_value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, UnknownCategoryReturnsEmptyList) {
    EXPECT_TRUE(controller.getItemsFromCategory("unknown").empty());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, ListIncludesPricesDescriptionAndSpecificFields) {
    auto items = controller.getItemsFromCategory("Item");
    ASSERT_EQ(1, items.size());
    EXPECT_TRUE(std::holds_alternative<std::monostate>(items[0].specificFields));

    auto weapons = controller.getItemsFromCategory("Weapon");
    ASSERT_EQ(1, weapons.size());
    EXPECT_EQ("swd001", weapons[0].id);
    EXPECT_EQ("Sword1", weapons[0].name);
    EXPECT_EQ("A sword", weapons[0].description);
    EXPECT_EQ(2, weapons[0].defaultBuyPrice);
    EXPECT_EQ(1, weapons[0].defaultSellPrice);
    ASSERT_TRUE(std::holds_alternative<WeaponListFields>(weapons[0].specificFields));
    const auto &weapon = std::get<WeaponListFields>(weapons[0].specificFields);
    EXPECT_EQ("Secondary hand", weapon.bodyPart);
    EXPECT_FLOAT_EQ(2.1F, weapon.attackGain);

    auto armors = controller.getItemsFromCategory("Armor");
    ASSERT_EQ(1, armors.size());
    ASSERT_TRUE(std::holds_alternative<ArmorListFields>(armors[0].specificFields));
    const auto &armor = std::get<ArmorListFields>(armors[0].specificFields);
    EXPECT_EQ("Hands", armor.bodyPart);
    EXPECT_FLOAT_EQ(3.2F, armor.defenseGain);

    auto potions = controller.getItemsFromCategory("Stats Item");
    ASSERT_EQ(1, potions.size());
    ASSERT_TRUE(std::holds_alternative<StatsItemListFields>(potions[0].specificFields));
    const auto &potion = std::get<StatsItemListFields>(potions[0].specificFields);
    EXPECT_EQ("Endurance", potion.statChanging);
    EXPECT_FLOAT_EQ(4.1F, potion.gain);
    EXPECT_TRUE(potion.limitOfOneApplied);
    EXPECT_EQ(0, potion.durationInTurns);
}

TEST_F(MainControllerSampleWithOneItem, IconsSkipMissingItemsAndMissingTextures) {
    EXPECT_TRUE(controller.getIconsFromItemIds({"missing", "ite001"},
                                               resources.path().toStdString()).empty());
}

class MainControllerWithTexture : public MainControllerLoadedGameData {
protected:
    void SetUp() override {
        ASSERT_NO_FATAL_FAILURE(MainControllerLoadedGameData::SetUp());
        ASSERT_TRUE(controller.getItemStoreForEdition()->getTextureContainerForEdition().addTexture(
            TextureInfo{"ItemsTile", "items.png", 512, 256, 32, 16}));
    }

    void expectOriginalTexture() {
        const auto &container = controller.getItemStore().getTextureContainer();
        ASSERT_EQ(1, container.getCount());
        const auto texture = container.getTextureByName("ItemsTile");
        ASSERT_TRUE(texture.has_value());
        EXPECT_EQ("items.png", texture->get().getFilename());
        EXPECT_EQ(512, texture->get().getWidth());
        EXPECT_EQ(256, texture->get().getHeight());
        EXPECT_EQ(32, texture->get().getTileWidth());
        EXPECT_EQ(16, texture->get().getTileHeight());
    }
};

TEST_F(MainControllerLoadedGameData, getItemStoreTexture_WithNoTexture_ReturnsNull) {
    EXPECT_EQ(nullptr, controller.getItemStoreTexture());
}

TEST_F(MainControllerWithTexture, getItemStoreTexture_ReturnsAllFieldsAsIndependentCopy) {
    auto texture = controller.getItemStoreTexture();
    ASSERT_NE(nullptr, texture);
    EXPECT_EQ("ItemsTile", texture->name);
    EXPECT_EQ("items.png", texture->filename);
    EXPECT_EQ(512, texture->width);
    EXPECT_EQ(256, texture->height);
    EXPECT_EQ(32, texture->tileWidth);
    EXPECT_EQ(16, texture->tileHeight);

    texture->name = "Changed";
    texture->filename = "changed.png";
    texture->width = 128;
    expectOriginalTexture();
}

TEST_F(MainControllerWithTexture, setItemStoreTexture_ReplacesAllFieldsWithoutAddingTexture) {
    auto replacement = std::make_unique<commoneditor::ui::TextureDTO>(
        commoneditor::ui::TextureDTO{"ItemsTile", "replacement.png", 128, 64, 8, 4});
    ASSERT_TRUE(controller.setItemStoreTexture(std::move(replacement))) << controller.getLastError();
    EXPECT_EQ(1, controller.getItemStore().getTextureContainer().getCount());
    const auto actual = controller.getItemStoreTexture();
    ASSERT_NE(nullptr, actual);
    EXPECT_EQ("ItemsTile", actual->name);
    EXPECT_EQ("replacement.png", actual->filename);
    EXPECT_EQ(128, actual->width);
    EXPECT_EQ(64, actual->height);
    EXPECT_EQ(8, actual->tileWidth);
    EXPECT_EQ(4, actual->tileHeight);
}

TEST_F(MainControllerLoadedGameData, setItemStoreTexture_NullInputLeavesEmptyStore) {
    EXPECT_FALSE(controller.setItemStoreTexture(nullptr));
    EXPECT_EQ("The texture info cannot be null", controller.getLastError());
    EXPECT_EQ(0, controller.getItemStore().getTextureContainer().getCount());
}

TEST_F(MainControllerWithTexture, setItemStoreTexture_NullInputPreservesExistingTexture) {
    EXPECT_FALSE(controller.setItemStoreTexture(nullptr));
    EXPECT_EQ("The texture info cannot be null", controller.getLastError());
    expectOriginalTexture();
}

TEST_F(MainControllerLoadedGameData, setItemStoreTexture_InvalidInputLeavesEmptyStore) {
    auto invalid = std::make_unique<commoneditor::ui::TextureDTO>(
        commoneditor::ui::TextureDTO{"ItemsTile", "items.png", 0, 256, 32, 16});
    EXPECT_FALSE(controller.setItemStoreTexture(std::move(invalid)));
    EXPECT_EQ("Unable to add the texture to the item store", controller.getLastError());
    EXPECT_EQ(0, controller.getItemStore().getTextureContainer().getCount());
}

TEST_F(MainControllerWithTexture, setItemStoreTexture_InvalidReplacementPreservesExistingTexture) {
    auto invalid = std::make_unique<commoneditor::ui::TextureDTO>(
        commoneditor::ui::TextureDTO{"ItemsTile", "replacement.png", 512, 256, 0, 16});
    EXPECT_FALSE(controller.setItemStoreTexture(std::move(invalid)));
    EXPECT_EQ("Unable to replace the texture in the item store", controller.getLastError());
    expectOriginalTexture();
}

TEST_F(MainControllerWithTexture, setItemStoreTexture_DuplicateNamePreservesBothTextures) {
    ASSERT_TRUE(controller.getItemStoreForEdition()->getTextureContainerForEdition().addTexture(
        TextureInfo{"OtherTile", "other.png", 64, 32, 8, 4}));
    auto duplicate = std::make_unique<commoneditor::ui::TextureDTO>(
        commoneditor::ui::TextureDTO{"OtherTile", "replacement.png", 128, 64, 16, 8});
    EXPECT_FALSE(controller.setItemStoreTexture(std::move(duplicate)));
    EXPECT_EQ("Unable to replace the texture in the item store", controller.getLastError());
    const auto &container = controller.getItemStore().getTextureContainer();
    ASSERT_EQ(2, container.getCount());
    const auto original = container.getTextureByName("ItemsTile");
    ASSERT_TRUE(original.has_value());
    EXPECT_EQ("items.png", original->get().getFilename());
    const auto other = container.getTextureByName("OtherTile");
    ASSERT_TRUE(other.has_value());
    EXPECT_EQ("other.png", other->get().getFilename());
    EXPECT_EQ(64, other->get().getWidth());
}

TEST_F(MainControllerLoadedGameData, StatsListIncludesVitalityStrengthAndDuration) {
    auto store = controller.getItemStoreForEdition();
    ASSERT_TRUE(store->addItem(std::make_shared<StatsItem>(StatsItemCreationInfo{
        "vit001", "Vitality potion", "ItemsTile", 0, "Restores vitality", 12, 6,
        Stats::Vitality, 7.5F, false, 3})));
    ASSERT_TRUE(store->addItem(std::make_shared<StatsItem>(StatsItemCreationInfo{
        "str001", "Strength potion", "ItemsTile", 1, "Adds strength", 20, 10,
        Stats::Strength, 2.5F, true, 5})));

    const auto items = controller.getItemsFromCategory("Stats Item");
    ASSERT_EQ(2, items.size());
    for (const auto &item : items) {
        SCOPED_TRACE(item.id);
        ASSERT_TRUE(std::holds_alternative<StatsItemListFields>(item.specificFields));
        const auto &fields = std::get<StatsItemListFields>(item.specificFields);
        if (item.id == "vit001") {
            EXPECT_EQ("Vitality", fields.statChanging);
            EXPECT_FLOAT_EQ(7.5F, fields.gain);
            EXPECT_FALSE(fields.limitOfOneApplied);
            EXPECT_EQ(3, fields.durationInTurns);
            EXPECT_EQ("Restores vitality", item.description);
            EXPECT_EQ(12, item.defaultBuyPrice);
            EXPECT_EQ(6, item.defaultSellPrice);
        } else {
            ASSERT_EQ("str001", item.id);
            EXPECT_EQ("Strength", fields.statChanging);
            EXPECT_FLOAT_EQ(2.5F, fields.gain);
            EXPECT_TRUE(fields.limitOfOneApplied);
            EXPECT_EQ(5, fields.durationInTurns);
        }
    }
}

TEST_F(MainControllerLoadedGameData, IconsExtractDistinctTilesFromSharedTexture) {
    int argc = 3;
    char program[] = "mainController_icons_test";
    char platformOption[] = "-platform";
    char platform[] = "offscreen";
    char *argv[] = {program, platformOption, platform, nullptr};
    QGuiApplication app(argc, argv);
    const auto texturesPath = std::filesystem::path(resources.path().toStdString()) / "textures";
    std::filesystem::create_directories(texturesPath);
    QImage image(32, 16, QImage::Format_ARGB32);
    image.fill(Qt::red);
    for (int y = 0; y < 16; ++y) {
        for (int x = 16; x < 32; ++x) {
            image.setPixelColor(x, y, Qt::blue);
        }
    }
    ASSERT_TRUE(image.save(QString::fromStdString((texturesPath / "items.png").string())));
    auto store = controller.getItemStoreForEdition();
    ASSERT_TRUE(store->getTextureContainerForEdition().addTexture(
        TextureInfo{"ItemsTile", "items.png", 32, 16, 16, 16}));
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"red001", "Red item", "ItemsTile", 0})));
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"blu001", "Blue item", "ItemsTile", 1})));

    const auto icons = controller.getIconsFromItemIds(
        {"red001", "blu001", "missing"}, resources.path().toStdString());
    ASSERT_EQ(2, icons.size());
    ASSERT_EQ(1, icons.count("red001"));
    ASSERT_EQ(1, icons.count("blu001"));
    const auto red = icons.at("red001").pixmap(16, 16).toImage();
    const auto blue = icons.at("blu001").pixmap(16, 16).toImage();
    ASSERT_FALSE(red.isNull());
    ASSERT_FALSE(blue.isNull());
    EXPECT_EQ(QSize(16, 16), red.size());
    EXPECT_EQ(QSize(16, 16), blue.size());
    EXPECT_EQ(QColor(Qt::red), red.pixelColor(8, 8));
    EXPECT_EQ(QColor(Qt::blue), blue.pixelColor(8, 8));
}
