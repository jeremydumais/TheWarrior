
#ifndef MAPEDITOR_SRC_EDITMERCHANTINVENTORYFORM_HPP_
#define MAPEDITOR_SRC_EDITMERCHANTINVENTORYFORM_HPP_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ui_editMerchantInventoryForm.h"
#include "editMerchantInventoryFormController.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "types.hpp"

class EditMerchantInventoryForm : public QDialog {
Q_OBJECT

 public:
    EditMerchantInventoryForm(QWidget *parent,
                        const std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> &itemStores,
                        const std::string &resourcesPath,
                        thewarrior::models::MerchantInventoryType inventoryType,
                        const std::optional<mapeditor::controllers::MerchantInventoryDTO> &selectedMerchantInventory,
                        const std::vector<std::string> &alreadyUsedMerchantInventoryNames);
    const mapeditor::controllers::MerchantInventoryDTO &getResult() const;

 private:
    Ui::editMerchantInventoryFormClass ui;
    mapeditor::controllers::EditMerchantInventoryFormController m_controller;
    mapeditor::controllers::MerchantInventoryDTO m_result;
    QTableWidgetKeyPressWatcher tableWidgetItemsKeyWatcher;
    void connectUIActions();
    void initializeItemsTable();
    void refreshItems();
    void onPushButtoOKItemClick();
    void onPushButtonAddItemClick();
    void onPushButtonRemoveItemClick();
    void onPushButtonOverridePriceClick();
    void toggleItemSelectionChanged();
    std::optional<std::string> getSelectedItemId() const;

};

#endif  // MAPEDITOR_SRC_EDITMERCHANTINVENTORYFORM_HPP_
