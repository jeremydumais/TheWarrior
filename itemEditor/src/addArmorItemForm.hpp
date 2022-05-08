#ifndef ADDARMORITEMFORM_H
#define ADDARMORITEMFORM_H

#include "ui_addArmorItemForm.h"
#include "addArmorItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class AddArmorItemForm : public QDialog
{
Q_OBJECT

public:
	AddArmorItemForm(QWidget *parent, 
				const std::string &resourcesPath,
				std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::addArmorItemFormClass ui;
	const std::string &m_resourcesPath;
	AddArmorItemController m_controller;
	void initializeComboBoxSlotInBodyPart();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // ADDARMORITEMFORM_H
