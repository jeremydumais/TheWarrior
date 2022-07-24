#include "editMonsterForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include <qtimer.h>

EditMonsterForm::EditMonsterForm(QWidget *parent, 
						 const std::string &resourcesPath,
						 std::shared_ptr<MonsterStore> monsterStore,
						 std::optional<std::string> monsterIdToEdit)
	: QDialog(parent),
	  ui(Ui::editMonsterFormClass()),
      m_controller(monsterStore),
      m_resourcesPath(resourcesPath),
      m_monsterIdToEdit(monsterIdToEdit)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MonsterEditor Icon.png"));
	
	connectUIActions();
	if (m_monsterIdToEdit.has_value()) {
		this->setWindowTitle("Edit monster");
		if (!loadExistingMonsterToForm()) {
			QTimer::singleShot(0, this, SLOT(close()));
		}
	}
}

void EditMonsterForm::connectUIActions() 
{
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonCancelClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonOKClick);
	connect(ui.pushButtonTexturePicker, &QPushButton::clicked, this, &EditMonsterForm::onPushButtonTexturePickerClick);
}

bool EditMonsterForm::loadExistingMonsterToForm()
{
	auto existingMonster = m_controller.getMonster(*m_monsterIdToEdit);
	if (existingMonster != nullptr) {
		ui.lineEditId->setText(existingMonster->id.c_str());
		ui.lineEditName->setText(existingMonster->name.c_str());
		ui.lineEditTextureName->setText(existingMonster->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(existingMonster->textureIndex);
	}
	else {
		ErrorMessage::show("Unable to load the selected monster");
		return false;
	}
	return true;
}

void EditMonsterForm::onPushButtonCancelClick()
{
	reject();
}

void EditMonsterForm::onPushButtonOKClick()
{
	auto monsterInfo = std::make_unique<MonsterDTO>();
	monsterInfo->id = ui.lineEditId->text().toStdString();
	monsterInfo->name = ui.lineEditName->text().toStdString();
	monsterInfo->textureName = ui.lineEditTextureName->text().toStdString();
	monsterInfo->textureIndex = ui.spinBoxTextureIndex->value();
    //TODO Add code to validate additional fields
    
    if (!m_monsterIdToEdit.has_value()) {
		if (!m_controller.addMonster(std::move(monsterInfo))) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	} 
    else {
		if (!m_controller.updateMonster(std::move(monsterInfo), *m_monsterIdToEdit)) {
			ErrorMessage::show(m_controller.getLastError());
			return;
		}
	}
	accept();
}

void EditMonsterForm::onPushButtonTexturePickerClick()
{
	/*auto result = showTexturePicker({ ui.lineEditTextureName->text().toStdString(),
									  ui.spinBoxTextureIndex->value() }, 
								    m_controller.getTextureContainer());
	if (result.has_value()) {
		ui.lineEditTextureName->setText(result->textureName.c_str());
		ui.spinBoxTextureIndex->setValue(result->textureIndex);
	}*/
}
