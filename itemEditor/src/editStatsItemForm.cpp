#include "editStatsItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

EditStatsItemForm::EditStatsItemForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<ItemStore> itemStore)
	: QDialog(parent),
	  ui(Ui::editStatsItemFormClass()),
	  m_resourcesPath(resourcesPath),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	initializeComboBoxStatChanging();
}

void EditStatsItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditStatsItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditStatsItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditStatsItemForm::onPushButtonTexturePickerClick);
}

void EditStatsItemForm::initializeComboBoxStatChanging()
{
	ui.comboBoxStatChanging->insertItem(0, "Vitality");
	ui.comboBoxStatChanging->insertItem(1, "Strength");
	ui.comboBoxStatChanging->insertItem(2, "Endurance");
}

void EditStatsItemForm::onPushButtonCancelClick()
{
	reject();
}

void EditStatsItemForm::onPushButtonOKClick()
{
	if (!m_controller.validateGain(ui.lineEditGain->text().toStdString()) ||
		!m_controller.validateDurationInSecs(ui.lineEditDurationInSecs->text().toStdString())) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	StatsItemCreationInfo itemInfo {
		ui.lineEditId->text().toStdString(),
		ui.lineEditName->text().toStdString(),
		ui.lineEditTextureName->text().toStdString(),
		ui.spinBoxTextureIndex->value(),
		static_cast<Stats>(ui.comboBoxStatChanging->currentIndex()),
		stof(ui.lineEditGain->text().toStdString()),
		ui.checkBoxLimitOfOneApplied->isChecked(),
		static_cast<unsigned int>(stoul(ui.lineEditDurationInSecs->text().toStdString())),
	};
	if (!m_controller.addItem(itemInfo)) {
		ErrorMessage::show(m_controller.getLastError());
		return;
	}
	accept();
}

void EditStatsItemForm::onPushButtonTexturePickerClick()
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