#include "editMerchantInventoryForm.hpp"
#include <fmt/format.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "addMerchantInventoryItemForm.hpp"
#include "editMerchantInventoryFormController.hpp"
#include "editMerchantInventoryItemPriceOverrideForm.hpp"
#include "errorMessage.hpp"
#include "itemPricingDTO.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "types.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::MerchantInventoryDTO;
using mapeditor::controllers::ItemPricingDTO;
using thewarrior::models::MerchantInventoryType;

EditMerchantInventoryForm::EditMerchantInventoryForm(QWidget *parent,
                                         const std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> &itemStores,
                                         const std::string &resourcesPath,
                                         MerchantInventoryType inventoryType,
                                         const std::optional<MerchantInventoryDTO> &selectedMerchantInventory,
                                         const std::vector<std::string> &alreadyUsedMerchantInventoryNames)
    : QDialog(parent),
    ui(Ui::editMerchantInventoryFormClass()),
    m_controller(itemStores, resourcesPath, inventoryType, selectedMerchantInventory, alreadyUsedMerchantInventoryNames) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    initializeItemsTable();
    refreshItems();
    ui.labelInventoryTypeValue->setText(inventoryType == MerchantInventoryType::WeaponsAndArmors ? "Weapons and Armors" : "Stats Items");
    if (selectedMerchantInventory.has_value()) {
        ui.lineEditName->setText(selectedMerchantInventory->name.c_str());
    }
}

const MerchantInventoryDTO &EditMerchantInventoryForm::getResult() const {
    return m_result;
}

void EditMerchantInventoryForm::connectUIActions() {
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMerchantInventoryForm::onPushButtoOKItemClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMerchantInventoryForm::reject);
    connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &EditMerchantInventoryForm::onPushButtonAddItemClick);
    connect(ui.pushButtonRemoveItem, &QPushButton::clicked, this, &EditMerchantInventoryForm::onPushButtonRemoveItemClick);
    connect(ui.pushButtonOverridePrice, &QPushButton::clicked, this, &EditMerchantInventoryForm::onPushButtonOverridePriceClick);
    connect(ui.tableWidgetItems->selectionModel(), &QItemSelectionModel::selectionChanged, this, &EditMerchantInventoryForm::toggleItemSelectionChanged);
    connect(ui.tableWidgetItems, &QTableWidget::itemDoubleClicked, this, &EditMerchantInventoryForm::onPushButtonOverridePriceClick);
}

void EditMerchantInventoryForm::initializeItemsTable() {
    ui.tableWidgetItems->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetItems->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetItems->setHorizontalHeaderItem(2, new QTableWidgetItem("Buy Price"));
    ui.tableWidgetItems->setHorizontalHeaderItem(3, new QTableWidgetItem("Sell Price"));
    ui.tableWidgetItems->setHorizontalHeaderItem(4, new QTableWidgetItem("Price Override?"));
    ui.tableWidgetItems->setColumnWidth(0, 150);
    ui.tableWidgetItems->setColumnWidth(1, 200);
    ui.tableWidgetItems->setColumnWidth(2, 100);
    ui.tableWidgetItems->setColumnWidth(3, 100);
    ui.tableWidgetItems->setColumnWidth(4, 150);
}

void EditMerchantInventoryForm::refreshItems() {
    ui.tableWidgetItems->model()->removeRows(0, ui.tableWidgetItems->rowCount());
    std::vector<std::string> itemIds;
    std::ranges::transform(m_controller.getItems(),
                           std::back_inserter(itemIds),
                           [](const mapeditor::controllers::MerchantInventoryItemDTO &item) -> std::string { return item.id; });
    auto itemIdsWithIcon = m_controller.getIconsFromItemIds(itemIds, m_controller.getResourcesPath());
    int index = 0;
    for (const auto &item : m_controller.getItems()) {
        ui.tableWidgetItems->insertRow(index);
        auto *idItem = new QTableWidgetItem(item.id.c_str());
        if (itemIdsWithIcon.contains(item.id)) {
            idItem->setIcon(itemIdsWithIcon[item.id]);
        }
        ui.tableWidgetItems->setItem(index, 0, idItem);
        const auto itemProperties = m_controller.getItemPropertiesById(item.id);
        if (itemProperties.has_value()) {
            ui.tableWidgetItems->setItem(index, 1, new QTableWidgetItem(itemProperties->get().name.c_str()));
            QString buyPrice = item.buyPriceOverride.has_value() ? 
                               QString::number(item.buyPriceOverride.value()) : 
                               QString::number(itemProperties->get().defaultBuyPrice);
            QString sellPrice = item.sellPriceOverride.has_value() ? 
                                QString::number(item.sellPriceOverride.value()) : 
                                QString::number(itemProperties->get().defaultSellPrice);
            ui.tableWidgetItems->setItem(index, 2, new QTableWidgetItem(buyPrice));
            ui.tableWidgetItems->setItem(index, 3, new QTableWidgetItem(sellPrice));
            ui.tableWidgetItems->setItem(index, 4, new QTableWidgetItem(item.buyPriceOverride.has_value() || item.sellPriceOverride.has_value() ? 
                                                                        "X" : ""));                                                                
        } 
        index++;
    }
    toggleItemSelectionChanged();
}

void EditMerchantInventoryForm::onPushButtoOKItemClick() {
    QString merchantInventoryName = ui.lineEditName->text().trimmed();
    if (merchantInventoryName.isEmpty()) {
        ErrorMessage::show("The name is required.");
        return;
    }
    const std::string merchantInventoryNameStr = merchantInventoryName.toStdString();
    if (m_controller.isMerchantInventoryNameAlreadyUsed(merchantInventoryNameStr)) {
        ErrorMessage::show(fmt::format("The merchant inventory name {} already exists in the list.", merchantInventoryNameStr));
        return;
    }

    if (m_controller.getItems().empty()) {
        ErrorMessage::show("The merchant inventory needs at least one item.");
        return;
    }

    m_result.name = merchantInventoryNameStr;
    m_result.inventoryType = m_controller.getInventoryType();
    for (const auto &dto : m_controller.getItems()) {
        m_result.items.push_back(dto);
    }

    accept();
}

void EditMerchantInventoryForm::onPushButtonAddItemClick() {
    AddMerchantInventoryItemForm formAddMerchantInventoryItem(this,
        m_controller.getItemStores(),
        m_controller.getResourcesPath(),
        m_controller.getInventoryType());
    UIUtils::centerToScreen(&formAddMerchantInventoryItem);
    if (formAddMerchantInventoryItem.exec() == QDialog::Accepted) {
        const auto itemId = formAddMerchantInventoryItem.getResult();
        if (!m_controller.addItem(itemId)) {
            ErrorMessage::show(m_controller.getLastError());
            return;
        }
        refreshItems();
    } 
}

void EditMerchantInventoryForm::onPushButtonRemoveItemClick() {
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the item {} from the merchant inventory?", itemId.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (!m_controller.removeItem(itemId.value())) {
                ErrorMessage::show(m_controller.getLastError());
                return;
            }
            refreshItems();
        }
    }
}

void EditMerchantInventoryForm::onPushButtonOverridePriceClick() {
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
        const auto itemPricing = m_controller.getItemPropertiesById(itemId.value());
        if (!itemPricing.has_value()) {
            ErrorMessage::show("Unable to find the selected item pricing element in the list.");
            return;
        }
        EditMerchantInventoryItemPriceOverrideForm editPriceOverrideForm(this, itemPricing->get());
        if (editPriceOverrideForm.exec() == QDialog::Accepted) {
            const auto &result = editPriceOverrideForm.getResult();
            m_controller.setOverridePrices(itemId.value(),
                                           result.buyPriceOverride,
                                           result.sellPriceOverride);
            refreshItems();
        }
    }
}

void EditMerchantInventoryForm::toggleItemSelectionChanged() {
    ui.pushButtonOverridePrice->setEnabled(!ui.tableWidgetItems->selectionModel()->selectedRows().empty());
}

std::optional<std::string> EditMerchantInventoryForm::getSelectedItemId() const {
    auto selectedRows = ui.tableWidgetItems->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}