#ifndef ADDITEMFORM_H
#define ADDITEMFORM_H

#include "ui_addItemForm.h"
#include "addItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class AddItemForm : public QDialog
{
Q_OBJECT

public:
	AddItemForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::addItemFormClass ui;
	const std::string &m_resourcesPath;
	AddItemController m_controller;
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // ADDITEMFORM_H
