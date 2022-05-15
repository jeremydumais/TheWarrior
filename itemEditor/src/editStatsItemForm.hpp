#ifndef EDITSTATSITEMFORM_H
#define EDITSTATSITEMFORM_H

#include "ui_editStatsItemForm.h"
#include "manageStatsItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class EditStatsItemForm : public QDialog
{
Q_OBJECT

public:
	EditStatsItemForm(QWidget *parent, 
					 const std::string &resourcesPath,
					 std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::editStatsItemFormClass ui;
	const std::string &m_resourcesPath;
	ManageStatsItemController m_controller;
	void initializeComboBoxStatChanging();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // EDITSTATSITEMFORM_H
