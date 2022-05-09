#include "addStatsItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

AddStatsItemForm::AddStatsItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::addStatsItemFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	initializeComboBoxStatChanging();
}

void AddStatsItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &AddStatsItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &AddStatsItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &AddStatsItemForm::onPushButtonTexturePickerClick);
}

void AddStatsItemForm::initializeComboBoxStatChanging()
{
	ui.comboBoxStatChanging->insertItem(0, "Vitality");
	ui.comboBoxStatChanging->insertItem(1, "Strength");
	ui.comboBoxStatChanging->insertItem(2, "Endurance");
}

void AddStatsItemForm::onPushButtonCancelClick()
{
	reject();
}

void AddStatsItemForm::onPushButtonOKClick()
{
	/*if (!m_controller.validateAttackGain(ui.lineEditAttackGain->text().toStdString())) {
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
	}*/
	accept();
}

void AddStatsItemForm::onPushButtonTexturePickerClick()
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