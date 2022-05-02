#include "addItemChooserForm.hpp"
#include "addItemForm.hpp"

AddItemChooserForm::AddItemChooserForm(QWidget *parent)
	: QDialog(parent),
	  ui(Ui::addItemChooserFormClass()),
	  result(ItemType::Item)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
}

void AddItemChooserForm::connectUIActions() 
{
	connect(ui.pushButtonClose, &QPushButton::clicked, this, &AddItemChooserForm::onPushButtonCloseClick);
	connect(ui.pushButtonAddItem, &QPushButton::clicked, this, &AddItemChooserForm::onPushButtonAddItemClick);
	connect(ui.pushButtonAddWeaponItem, &QPushButton::clicked, this, &AddItemChooserForm::onPushButtonAddWeaponItemClick);
	connect(ui.pushButtonAddArmorItem, &QPushButton::clicked, this, &AddItemChooserForm::onPushButtonAddArmorItemClick);
	connect(ui.pushButtonAddStatsItem, &QPushButton::clicked, this, &AddItemChooserForm::onPushButtonAddStatsItemClick);
}

void AddItemChooserForm::onPushButtonCloseClick()
{
	reject();
}
	
void AddItemChooserForm::onPushButtonAddItemClick()
{
	result = ItemType::Item;
	accept();
}

void AddItemChooserForm::onPushButtonAddWeaponItemClick()
{
	result = ItemType::Weapon;
	accept();
}

void AddItemChooserForm::onPushButtonAddArmorItemClick()
{
	result = ItemType::Armor;
	accept();
}

void AddItemChooserForm::onPushButtonAddStatsItemClick()
{
	result = ItemType::StatsItem;
	accept();
}
