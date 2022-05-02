#include "addItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

AddItemForm::AddItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::addItemFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
}

void AddItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &AddItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &AddItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &AddItemForm::onPushButtonTexturePickerClick);
}

void AddItemForm::onPushButtonCancelClick()
{
	close();
}

void AddItemForm::onPushButtonOKClick()
{
	ErrorMessage::show("Ok!");
}

void AddItemForm::onPushButtonTexturePickerClick()
{
	TexturePickerForm texturePickerForm(this, 
										m_resourcesPath,
										m_controller.getTextureContainer());
	texturePickerForm.exec();
}