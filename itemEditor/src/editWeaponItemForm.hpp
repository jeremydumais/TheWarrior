#ifndef EDITWEAPONITEMFORM_H
#define EDITWEAPONITEMFORM_H

#include "ui_editWeaponItemForm.h"
#include "manageWeaponItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class EditWeaponItemForm : public QDialog
{
Q_OBJECT

public:
	EditWeaponItemForm(QWidget *parent, 
					  const std::string &resourcesPath,
					  std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::editWeaponItemFormClass ui;
	const std::string &m_resourcesPath;
	ManageWeaponItemController m_controller;
	void initializeComboBoxSlotInBodyPart();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITWEAPONITEMFORM_H
