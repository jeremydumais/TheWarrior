
#ifndef MAPEDITOR_SRC_ADDMERCHANTINVENTORYITEMFORM_HPP_
#define MAPEDITOR_SRC_ADDMERCHANTINVENTORYITEMFORM_HPP_

#include "qicon.h"
#include <memory>
#include <optional>
#include <string>
#include "addMerchantInventoryItemFormController.hpp"
#include "merchantInventory.hpp"
#include "ui_addMerchantInventoryItemForm.h"
#include "types.hpp"

class AddMerchantInventoryItemForm : public QDialog {
Q_OBJECT

 public:
    AddMerchantInventoryItemForm(QWidget *parent,
                                 const std::shared_ptr<mapeditor::controllers::ContainerOfItemStore> &itemStores,
                                 const std::string &resourcesPath,
                                 thewarrior::models::MerchantInventoryType inventoryType);
    std::string getResult() const;

 private:
    Ui::addMerchantInventoryItemFormClass ui;
    mapeditor::controllers::AddMerchantInventoryItemFormController m_controller;
    std::string m_result;
    void initializeCategoriesTableControl();
    void initializeItemsTableControl();
    void connectUIActions();
    void refreshCategoriesTable();
    void refreshItemsTable();
    void onTableWidgetItemCategoriesSelectionChanged();
    void onPushButtonOKClick();
    std::optional<std::string> getSelectedItemId() const;
    static QIcon getCategoryIcon(const std::string &categoryName);
};

#endif  // MAPEDITOR_SRC_ADDMERCHANTINVENTORYITEMFORM_HPP_
