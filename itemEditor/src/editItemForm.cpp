#include "editItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

EditItemForm::EditItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::editItemFormClass()),
	  m_lastError(""),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
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
	if (!m_controller.addItem(itemInfo)) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditItemForm::onPushButtonTexturePickerClick()
{
	TexturePickerForm texturePickerForm(this, 
										m_resourcesPath,
										m_controller.getTextureContainer());
	auto selectedTexture = ui.lineEditTextureName->text();
	if (!selectedTexture.trimmed().isEmpty()) {
		texturePickerForm.setCurrentSelection(selectedTexture.toStdString(),
											  ui.spinBoxTextureIndex->value());
	}
	if (texturePickerForm.exec() == QDialog::Accepted) {
		const auto &result = texturePickerForm.getResult();
		ui.lineEditTextureName->setText(result.textureName.c_str());
		ui.spinBoxTextureIndex->setValue(result.textureIndex);
	}
}