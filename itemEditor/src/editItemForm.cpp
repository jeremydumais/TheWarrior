#include "editItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include <qtimer.h>

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
		if (!loadExistingItemToForm()) {
			QTimer::singleShot(0, this, SLOT(close()));
		}
	}
}

void EditItemForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditItemForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditItemForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditItemForm::onPushButtonTexturePickerClick);
}

bool EditItemForm::loadExistingItemToForm()
{
	auto existingItem = m_controller.getItem(*m_itemIdToEdit);
	if (existingItem != nullptr) {
		ui.lineEditId->setText(existingItem->id.c_str());
		ui.lineEditName->setText(existingItem->name.c_str());
		ui.lineEditTextureName->setText(existingItem->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(existingItem->textureIndex);
	}
	else {
		ErrorMessage::show("Unable to load the selected item");
		return false;
	}
	return true;
}

void EditItemForm::onPushButtonCancelClick()
{
	reject();
}

void EditItemForm::onPushButtonOKClick()
{
	auto itemInfo = std::make_unique<ItemDTO>();
	itemInfo->id = ui.lineEditId->text().toStdString();
	itemInfo->name = ui.lineEditName->text().toStdString();
	itemInfo->textureName = ui.lineEditTextureName->text().toStdString();
	itemInfo->textureIndex = ui.spinBoxTextureIndex->value();
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