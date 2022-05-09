#ifndef ADDSTATSITEMFORM_H
#define ADDSTATSITEMFORM_H

#include "ui_addStatsItemForm.h"
#include "addStatsItemController.hpp"
#include "itemStore.hpp"
#include <memory>
#include <string>

class AddStatsItemForm : public QDialog
{
Q_OBJECT

public:
	AddStatsItemForm(QWidget *parent, 
					 const std::string &resourcesPath,
					 std::shared_ptr<ItemStore> itemStore);
	void connectUIActions();
private:
	Ui::addStatsItemFormClass ui;
	const std::string &m_resourcesPath;
	AddStatsItemController m_controller;
	void initializeComboBoxStatChanging();
	void onPushButtonCancelClick();
	void onPushButtonOKClick();
	void onPushButtonTexturePickerClick();
};

#endif // ADDSTATSITEMFORM_H
