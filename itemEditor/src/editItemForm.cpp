#include "editItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

EditItemForm::EditItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore,
						 std::optional<std::string> itemIdToEdit)
	: EditItemFormBase(parent, resourcesPath, itemIdToEdit),
	  ui(Ui::editItemFormClass()),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	
	connectUIActions();
	if (m_itemIdToEdit.has_value()) {
		this->setWindowTitle("Edit item");
		//TODO if edit mode fetch item and set ui fields
	}
}

void EditItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditItemForm::onPushButtonTexturePickerClick);
}

void EditItemForm::onPushButtonCancelClick()
{
	reject();
}

void EditItemForm::onPushButtonOKClick()
{
	ItemCreationInfo itemInfo {
		ui.lineEditId->text().toStdString(),
		ui.lineEditName->text().toStdString(),
		ui.lineEditTextureName->text().toStdString(),
		ui.spinBoxTextureIndex->value()
	};
	//TODO if add or edit mode
	if (!m_controller.addItem(itemInfo)) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditItemForm::onPushButtonTexturePickerClick()
{
	auto result = showTexturePicker({ ui.lineEditTextureName->text().toStdString(),
									  ui.spinBoxTextureIndex->value() }, 
								    m_controller.getTextureContainer());
	if (result.has_value()) {
		ui.lineEditTextureName->setText(result->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(result->textureIndex);
	}
}