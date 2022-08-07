#include "addItemChooserForm.hpp"

AddItemChooserForm::AddItemChooserForm(QWidget *parent)
	: QDialog(parent),
	  ui(Ui::addItemChooserFormClass()),
	  m_result(ItemType::Item)
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

ItemType AddItemChooserForm::getResult() const
{
    return m_result;
}

void AddItemChooserForm::onPushButtonCloseClick()
{
	reject();
}

void AddItemChooserForm::onPushButtonAddItemClick()
{
	m_result = ItemType::Item;
	accept();
}

void AddItemChooserForm::onPushButtonAddWeaponItemClick()
{
	m_result = ItemType::Weapon;
	accept();
}

void AddItemChooserForm::onPushButtonAddArmorItemClick()
{
	m_result = ItemType::Armor;
	accept();
}

void AddItemChooserForm::onPushButtonAddStatsItemClick()
{
	m_result = ItemType::StatsItem;
	accept();
}
