#include "editArmorItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

EditArmorItemForm::EditArmorItemForm(QWidget *parent, 
						 		   const std::string &resourcesPath,
						 		   std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::editArmorItemFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	initializeComboBoxSlotInBodyPart();
}

void EditArmorItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditArmorItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditArmorItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditArmorItemForm::onPushButtonTexturePickerClick);
}

void EditArmorItemForm::initializeComboBoxSlotInBodyPart()
{
	ui.comboBoxSlotInBodyPart->insertItem(0, "Head");
	ui.comboBoxSlotInBodyPart->insertItem(1, "Upper body");
	ui.comboBoxSlotInBodyPart->insertItem(2, "Lower body");
	ui.comboBoxSlotInBodyPart->insertItem(3, "Hands");
	ui.comboBoxSlotInBodyPart->insertItem(4, "Feets");
}

void EditArmorItemForm::onPushButtonCancelClick()
{
	reject();
}

void EditArmorItemForm::onPushButtonOKClick()
{
	if (!m_controller.validateDefenseGain(ui.lineEditDefenseGain->text().toStdString())) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	ArmorItemCreationInfo itemInfo {
		ui.lineEditId->text().toStdString(),
		ui.lineEditName->text().toStdString(),
		ui.lineEditTextureName->text().toStdString(),
		ui.spinBoxTextureIndex->value(),
		stof(ui.lineEditDefenseGain->text().toStdString()),
		static_cast<ArmorBodyPart>(ui.comboBoxSlotInBodyPart->currentIndex())
	};
	if (!m_controller.addItem(itemInfo)) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditArmorItemForm::onPushButtonTexturePickerClick()
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