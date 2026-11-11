#ifndef ADDITEMCHOOSERFORM_H
#define ADDITEMCHOOSERFORM_H

#include "ui_addItemChooserForm.h"
#include "itemType.hpp"

class AddItemChooserForm : public QDialog
{
Q_OBJECT

public:
	AddItemChooserForm(QWidget *parent);
	void connectUIActions();
    thewarrior::models::ItemType getResult() const;
private:
	Ui::addItemChooserFormClass ui;
	thewarrior::models::ItemType m_result;
	void onPushButtonCloseClick();
	void onPushButtonAddItemClick();
	void onPushButtonAddWeaponItemClick();
	void onPushButtonAddArmorItemClick();
	void onPushButtonAddStatsItemClick();
};

#endif // ADDITEMCHOOSERFORM_H
