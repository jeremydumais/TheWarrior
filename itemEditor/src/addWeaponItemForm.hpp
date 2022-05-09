#ifndef ADDWEAPONITEMFORM_H
#define ADDWEAPONITEMFORM_H

#include "ui_addWeaponItemForm.h"
#include "addWeaponItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class AddWeaponItemForm : public QDialog
{
Q_OBJECT

public:
	AddWeaponItemForm(QWidget *parent, 
					  const std::string &resourcesPath,
					  std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::addWeaponItemFormClass ui;
	const std::string &m_resourcesPath;
	AddWeaponItemController m_controller;
	void initializeComboBoxSlotInBodyPart();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // ADDWEAPONITEMFORM_H
