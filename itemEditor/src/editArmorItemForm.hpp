#ifndef EDITARMORITEMFORM_H
#define EDITARMORITEMFORM_H

#include "ui_editArmorItemForm.h"
#include "manageArmorItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class EditArmorItemForm : public QDialog
{
Q_OBJECT

public:
	EditArmorItemForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::editArmorItemFormClass ui;
	const std::string &m_resourcesPath;
	ManageArmorItemController m_controller;
	void initializeComboBoxSlotInBodyPart();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITARMORITEMFORM_H
