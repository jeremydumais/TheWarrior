#ifndef EDITSTATSITEMFORM_H
#define EDITSTATSITEMFORM_H

#include "ui_editStatsItemForm.h"
#include "editItemFormBase.hpp"
#include "manageStatsItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <optional>
#include <string>

class EditStatsItemForm : public EditItemFormBase
{
Q_OBJECT

public:
    EditStatsItemForm(QWidget *parent,
            const std::string &resourcesPath,
            std::shared_ptr<ItemStore> itemStore,
            std::optional<std::string> itemIdToEdit);
    void connectUIActions();
private:
    Ui::editStatsItemFormClass ui;
    itemeditor::controllers::ManageStatsItemController m_controller;
    void initializeComboBoxStatChanging();
    bool loadExistingItemToForm();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonTexturePickerClick();
};

#endif // EDITSTATSITEMFORM_H
