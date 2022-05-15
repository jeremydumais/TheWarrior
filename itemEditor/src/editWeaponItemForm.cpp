#include "editWeaponItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

EditWeaponItemForm::EditWeaponItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::editWeaponItemFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	initializeComboBoxSlotInBodyPart();
}

void EditWeaponItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditWeaponItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditWeaponItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditWeaponItemForm::onPushButtonTexturePickerClick);
}

void EditWeaponItemForm::initializeComboBoxSlotInBodyPart()
{
	ui.comboBoxSlotInBodyPart->insertItem(0, "Main hand");
	ui.comboBoxSlotInBodyPart->insertItem(1, "Secondary hand");
}

void EditWeaponItemForm::onPushButtonCancelClick()
{
	reject();
}

void EditWeaponItemForm::onPushButtonOKClick()
{
	if (!m_controller.validateAttackGain(ui.lineEditAttackGain->text().toStdString())) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	WeaponItemCreationInfo itemInfo {
		ui.lineEditId->text().toStdString(),
		ui.lineEditName->text().toStdString(),
		ui.lineEditTextureName->text().toStdString(),
		ui.spinBoxTextureIndex->value(),
		stof(ui.lineEditAttackGain->text().toStdString()),
		static_cast<WeaponBodyPart>(ui.comboBoxSlotInBodyPart->currentIndex())
	};
	if (!m_controller.addItem(itemInfo)) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditWeaponItemForm::onPushButtonTexturePickerClick()
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