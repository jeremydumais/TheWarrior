#include "editArmorItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include <memory>
#include <string>

EditArmorItemForm::EditArmorItemForm(QWidget *parent, 
						 		   const std::string &resourcesPath,
						 		   std::shared_ptr<ItemStore> itemStore,
								   std::optional<std::string> itemIdToEdit)
	: EditItemFormBase(parent, resourcesPath, itemIdToEdit),
	  ui(Ui::editArmorItemFormClass()),
	  m_controller(itemStore)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/ItemEditor Icon.png"));
	connectUIActions();
	initializeComboBoxSlotInBodyPart();
	if (m_itemIdToEdit.has_value()) {
		this->setWindowTitle("Edit armor item");
		if (!loadExistingItemToForm()) {
			QTimer::singleShot(0, this, SLOT(close()));
		}
	}
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
	ui.comboBoxSlotInBodyPart->insertItem(4, "Feet");
	ui.comboBoxSlotInBodyPart->insertItem(5, "Secondary hand");
}

bool EditArmorItemForm::loadExistingItemToForm()
{
	auto existingItem = m_controller.getItem(*m_itemIdToEdit);
	if (existingItem != nullptr) {
		auto *armorDTO = dynamic_cast<ArmorItemDTO *>(existingItem.get());
		if (armorDTO != nullptr) {
			ui.lineEditId->setText(armorDTO->id.c_str());
			ui.lineEditName->setText(armorDTO->name.c_str());
			ui.lineEditTextureName->setText(armorDTO->textureName.c_str());
			ui.spinBoxTextureIndex->setValue(armorDTO->textureIndex);
			ui.lineEditOptionalDescription->setText(armorDTO->optionalDescription.c_str());
			ui.lineEditDefenseGain->setText(std::to_string(armorDTO->defenseGain).c_str());
			ui.comboBoxSlotInBodyPart->setCurrentIndex(armorDTO->slotInBodyPartIndex);
		}
		else {
			ErrorMessage::show("Unable to cast the selected item to armor type");
			return false;
		}
	}
	else {
		ErrorMessage::show("Unable to load the selected item");
		return false;
	}
	return true;
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
	auto itemInfo = std::make_unique<ArmorItemDTO>();
	itemInfo->id = ui.lineEditId->text().toStdString();
	itemInfo->name = ui.lineEditName->text().toStdString();
	itemInfo->textureName = ui.lineEditTextureName->text().toStdString();
	itemInfo->textureIndex = ui.spinBoxTextureIndex->value();
	itemInfo->optionalDescription = ui.lineEditOptionalDescription->text().toStdString();
	itemInfo->defenseGain = stof(ui.lineEditDefenseGain->text().toStdString());
	itemInfo->slotInBodyPartIndex = ui.comboBoxSlotInBodyPart->currentIndex();
	if (!m_itemIdToEdit.has_value()) {
		if (!m_controller.addItem(std::move(itemInfo))) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	}
	else {
		if (!m_controller.updateItem(std::move(itemInfo), *m_itemIdToEdit)) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	}
	accept();
}

void EditArmorItemForm::onPushButtonTexturePickerClick()
{
	auto result = showTexturePicker({ ui.lineEditTextureName->text().toStdString(),
									  ui.spinBoxTextureIndex->value() }, 
								    m_controller.getTextureContainer());
	if (result.has_value()) {
		ui.lineEditTextureName->setText(result->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(result->textureIndex);
	}
}