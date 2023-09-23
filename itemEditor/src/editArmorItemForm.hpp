#ifndef EDITARMORITEMFORM_H
#define EDITARMORITEMFORM_H

#include "ui_editArmorItemForm.h"
#include "editItemFormBase.hpp"
#include "manageArmorItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <optional>
#include <string>

class EditArmorItemForm :  public EditItemFormBase
{
Q_OBJECT

public:
	EditArmorItemForm(QWidget *parent,
				const std::string &resourcesPath,
				std::shared_ptr<thewarrior::models::ItemStore> itemStore,
				std::optional<std::string> itemIdToEdit);
	void connectUIActions();
private:
	Ui::editArmorItemFormClass ui;
    itemeditor::controllers::ManageArmorItemController m_controller;
	void initializeComboBoxSlotInBodyPart();
	bool loadExistingItemToForm();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITARMORITEMFORM_H
