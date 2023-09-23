#include "editWeaponItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"
#include <boost/lexical_cast.hpp>

using namespace commoneditor::ui;
using namespace itemeditor::controllers;
using namespace thewarrior::models;

EditWeaponItemForm::EditWeaponItemForm(QWidget *parent,
        const std::string &resourcesPath,
        std::shared_ptr<ItemStore> itemStore,
        std::optional<std::string> itemIdToEdit)
    : EditItemFormBase(parent, resourcesPath, itemIdToEdit),
    ui(Ui::editWeaponItemFormClass()),
    m_controller(itemStore)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
    connectUIActions();
    initializeComboBoxSlotInBodyPart();
    if (m_itemIdToEdit.has_value()) {
        this->setWindowTitle("Edit weapon item");
        if (!loadExistingItemToForm()) {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
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

bool EditWeaponItemForm::loadExistingItemToForm()
{
    auto existingItem = m_controller.getItem(*m_itemIdToEdit);
    if (existingItem != nullptr) {
        auto *weaponDTO = dynamic_cast<WeaponItemDTO *>(existingItem.get());
        if (weaponDTO != nullptr) {
            ui.lineEditId->setText(weaponDTO->id.c_str());
            ui.lineEditName->setText(weaponDTO->name.c_str());
            ui.lineEditTextureName->setText(weaponDTO->textureName.c_str());
            ui.spinBoxTextureIndex->setValue(weaponDTO->textureIndex);
            ui.lineEditOptionalDescription->setText(weaponDTO->optionalDescription.c_str());
            ui.lineEditAttackGain->setText(std::to_string(weaponDTO->attackGain).c_str());
            ui.comboBoxSlotInBodyPart->setCurrentIndex(weaponDTO->slotInBodyPartIndex);
        }
        else {
            ErrorMessage::show("Unable to cast the selected item to weapon type");
            return false;
        }
    }
    else {
        ErrorMessage::show("Unable to load the selected item");
        return false;
    }
    return true;
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
    auto itemInfo = std::make_unique<WeaponItemDTO>();
    itemInfo->id = ui.lineEditId->text().toStdString();
    itemInfo->name = ui.lineEditName->text().toStdString();
    itemInfo->textureName = ui.lineEditTextureName->text().toStdString();
    itemInfo->textureIndex = ui.spinBoxTextureIndex->value();
    itemInfo->optionalDescription = ui.lineEditOptionalDescription->text().toStdString();
    itemInfo->attackGain = stof(ui.lineEditAttackGain->text().toStdString());
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

void EditWeaponItemForm::onPushButtonTexturePickerClick()
{
    auto result = showTexturePicker({ ui.lineEditTextureName->text().toStdString(),
            ui.spinBoxTextureIndex->value() },
            m_controller.getTextureContainer());
    if (result.has_value()) {
        ui.lineEditTextureName->setText(result->textureName.c_str());
        ui.spinBoxTextureIndex->setValue(result->textureIndex);
    }
}
