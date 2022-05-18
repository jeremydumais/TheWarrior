#ifndef EDITITEMFORM_H
#define EDITITEMFORM_H

#include "ui_editItemForm.h"
#include "editItemFormBase.hpp"
#include "manageItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <optional>
#include <string>

class EditItemForm : public EditItemFormBase
{
Q_OBJECT

public:
	EditItemForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<ItemStore> itemStore,
				std::optional<std::string> itemIdToEdit);
	void connectUIActions();
	
private:
	Ui::editItemFormClass ui;
	ManageItemController m_controller;
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITITEMFORM_H