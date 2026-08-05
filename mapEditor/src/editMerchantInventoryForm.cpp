#include "editMerchantInventoryForm.hpp"
#include <fmt/format.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "addMerchantInventoryItemForm.hpp"
#include "editMerchantInventoryFormController.hpp"
#include "errorMessage.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "types.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using thewarrior::models::MerchantInventoryType;

EditMerchantInventoryForm::EditMerchantInventoryForm(QWidget *parent,
                                         const std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> &itemStores,
                                         const std::string &resourcesPath,
                                         MerchantInventoryType inventoryType,
                                         const std::optional<mapeditor::controllers::MerchantInventoryDTO> &selectedMerchantInventory,
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
}

void EditMerchantInventoryForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMerchantInventoryForm::reject);
    connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &EditMerchantInventoryForm::onPushButtonAddItemClick);
}

void EditMerchantInventoryForm::initializeItemsTable() {
    ui.tableWidgetItems->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetItems->setHorizontalHeaderItem(1, new QTableWidgetItem("Buy Price"));
    ui.tableWidgetItems->setHorizontalHeaderItem(2, new QTableWidgetItem("Sell Price"));
    ui.tableWidgetItems->setHorizontalHeaderItem(3, new QTableWidgetItem("Price Override?"));
    ui.tableWidgetItems->setColumnWidth(0, 150);
    ui.tableWidgetItems->setColumnWidth(1, 100);
    ui.tableWidgetItems->setColumnWidth(2, 100);
    ui.tableWidgetItems->setColumnWidth(3, 150);
}

void EditMerchantInventoryForm::refreshItems() {
    ui.tableWidgetItems->model()->removeRows(0, ui.tableWidgetItems->rowCount());
    int index = 0;
    for (const auto &item : m_controller.getItems()) {
        ui.tableWidgetItems->insertRow(index);
        auto *idItem = new QTableWidgetItem(item.itemId.c_str());
        /*if (itemIdsWithIcon.contains(item.itemId)) {
            idItem->setIcon(itemIdsWithIcon[item.itemId]);
        }*/
        ui.tableWidgetItems->setItem(index, 0, idItem);
        /*ui.tableWidgetItems->setItem(index, 1, new QTableWidgetItem(item.buyPriceOverride()));
        ui.tableWidgetItems->setItem(index, 2, new QTableWidgetItem(item.description.c_str()));*/
        index++;
    }
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