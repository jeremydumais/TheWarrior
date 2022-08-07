#ifndef EDITWEAPONITEMFORM_H
#define EDITWEAPONITEMFORM_H

#include "ui_editWeaponItemForm.h"
#include "editItemFormBase.hpp"
#include "manageWeaponItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <optional>
#include <string>

class EditWeaponItemForm : public EditItemFormBase
{
Q_OBJECT

public:
    EditWeaponItemForm(QWidget *parent,
            const std::string &resourcesPath,
            std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            std::optional<std::string> itemIdToEdit);
    void connectUIActions();
private:
    Ui::editWeaponItemFormClass ui;
    itemeditor::controllers::ManageWeaponItemController m_controller;
    void initializeComboBoxSlotInBodyPart();
    bool loadExistingItemToForm();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonTexturePickerClick();
};

#endif // EDITWEAPONITEMFORM_H
