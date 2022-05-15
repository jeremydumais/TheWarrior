#ifndef EDITITEMFORM_H
#define EDITITEMFORM_H

#include "ui_editItemForm.h"
#include "manageItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class EditItemForm : public QDialog
{
Q_OBJECT

public:
	EditItemForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
	
private:
	Ui::editItemFormClass ui;
	std::string m_lastError;
	const std::string &m_resourcesPath;
	ManageItemController m_controller;
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITITEMFORM_H
