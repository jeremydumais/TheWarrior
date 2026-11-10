#include <qicon.h>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include "addMerchantInventoryItemForm.hpp"
#include "errorMessage.hpp"
#include "merchantInventory.hpp"
#include "types.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::ContainerOfItemStore;
using thewarrior::models::MerchantInventoryType;

AddMerchantInventoryItemForm::AddMerchantInventoryItemForm(QWidget *parent,
                                 const std::shared_ptr<ContainerOfItemStore> &itemStores,
                                 const std::string &resourcesPath,
                                 MerchantInventoryType inventoryType)
: QDialog(parent),
ui(Ui::addMerchantInventoryItemFormClass()),
m_controller(itemStores, resourcesPath, inventoryType) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    initializeCategoriesTableControl();
    initializeItemsTableControl();
    connectUIActions();
    refreshCategoriesTable();
}

std::string AddMerchantInventoryItemForm::getResult() const {
    return m_result;
}

void AddMerchantInventoryItemForm::initializeCategoriesTableControl() {
    ui.tableWidgetItemCategories->setFixedWidth(300);
    ui.tableWidgetItemCategories->setHorizontalHeaderItem(0, new QTableWidgetItem("Categories"));
}

void AddMerchantInventoryItemForm::initializeItemsTableControl() {
    ui.tableWidgetItems->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetItems->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetItems->setHorizontalHeaderItem(2, new QTableWidgetItem("Description"));
    ui.tableWidgetItems->setColumnWidth(1, 200);
}

void AddMerchantInventoryItemForm::connectUIActions() {
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &AddMerchantInventoryItemForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &AddMerchantInventoryItemForm::reject);
    connect(ui.tableWidgetItemCategories->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AddMerchantInventoryItemForm::onTableWidgetItemCategoriesSelectionChanged);
    connect(ui.tableWidgetItems, &QTableWidget::itemDoubleClicked, this, &AddMerchantInventoryItemForm::onPushButtonOKClick);
}

void AddMerchantInventoryItemForm::refreshCategoriesTable() {
    // Get the selected category
    std::string selectedCategory;
    auto currentCategoryItems = ui.tableWidgetItemCategories->selectedItems();
    if (currentCategoryItems.length() == 1) {
        selectedCategory = currentCategoryItems[0]->text().toStdString();
    }
    // Clear and regenerate the category list
    ui.tableWidgetItemCategories->model()->removeRows(0, ui.tableWidgetItemCategories->rowCount());
    int index = 0;
    for (const std::string &category : m_controller.getItemCategories()) {
        ui.tableWidgetItemCategories->insertRow(index);
        auto *categoryRow = new QTableWidgetItem(category.c_str());
        categoryRow->setIcon(getCategoryIcon(category));
        auto font = categoryRow->font();
        font.setPointSize(16);
        categoryRow->setFont(font);
        ui.tableWidgetItemCategories->setItem(index, 0, categoryRow);
        if (category == selectedCategory) {
            categoryRow->setSelected(true);
        }
        index++;
    }
}

void AddMerchantInventoryItemForm::refreshItemsTable() {
    ui.tableWidgetItems->model()->removeRows(0, ui.tableWidgetItems->rowCount());
    auto currentItemCategorySelections = ui.tableWidgetItemCategories->selectionModel()->selectedRows();
    if (currentItemCategorySelections.length() == 1) {
        auto selectedItemCategory = currentItemCategorySelections[0].data().toString().toStdString();
        auto itemsToDisplay = m_controller.getItemsFromCategory(selectedItemCategory);
        std::vector<std::string> itemIds;
        std::ranges::transform(itemsToDisplay,
                std::back_inserter(itemIds),
                [](const mapeditor::controllers::AddMerchantInventoryItemFormController::ItemListDisplay &itemDisplay) -> std::string { return itemDisplay.id; });
        auto itemIdsWithIcon = m_controller.getIconsFromItemIds(itemIds, m_controller.getResourcesPath());
        int index = 0;
        for (const auto &item : itemsToDisplay) {
            ui.tableWidgetItems->insertRow(index);
            auto *idItem = new QTableWidgetItem(item.id.c_str());
            if (itemIdsWithIcon.contains(item.id)) {
                idItem->setIcon(itemIdsWithIcon[item.id]);
            }
            ui.tableWidgetItems->setItem(index, 0, idItem);
            ui.tableWidgetItems->setItem(index, 1, new QTableWidgetItem(item.name.c_str()));
            ui.tableWidgetItems->setItem(index, 2, new QTableWidgetItem(item.description.c_str()));
            index++;
        }
    }
}

void AddMerchantInventoryItemForm::onTableWidgetItemCategoriesSelectionChanged() {
    refreshItemsTable();
}

void AddMerchantInventoryItemForm::onPushButtonOKClick() {
    auto itemId = getSelectedItemId();
    if (!itemId.has_value()) {
        ErrorMessage::show("You must select an item from the list");
        return;
    }
    m_result = itemId.value();
    accept();
}

std::optional<std::string> AddMerchantInventoryItemForm::getSelectedItemId() const {
    auto selectedRows = ui.tableWidgetItems->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}

QIcon AddMerchantInventoryItemForm::getCategoryIcon(const std::string &categoryName) {
    if (categoryName == "Weapon") {
        return QIcon(":/sword.png");
    }
    if (categoryName == "Armor") {
        return QIcon(":/shield.png");
    } 
    if (categoryName == "StatsItem") {
        return QIcon(":/statsitem.png");
    } 
    return QIcon(":/item.png");
}

