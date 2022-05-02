#ifndef ADDITEMCHOOSERFORM_H
#define ADDITEMCHOOSERFORM_H

#include "ui_addItemChooserForm.h"

enum class ItemType 
{
	Item,
	Weapon,
	Armor,
	StatsItem
};

class AddItemChooserForm : public QDialog
{
Q_OBJECT

public:
	AddItemChooserForm(QWidget *parent);
	void connectUIActions();

private:
	Ui::addItemChooserFormClass ui;
	ItemType result;
	void onPushButtonCloseClick();
	void onPushButtonAddItemClick();
	void onPushButtonAddWeaponItemClick();
	void onPushButtonAddArmorItemClick();
	void onPushButtonAddStatsItemClick();
};

#endif // ADDITEMCHOOSERFORM_H
