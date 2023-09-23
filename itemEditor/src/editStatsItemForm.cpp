#include "editStatsItemForm.hpp"
#include "errorMessage.hpp"
#include "texturePickerForm.hpp"

using namespace commoneditor::ui;
using namespace itemeditor::controllers;
using namespace thewarrior::models;

EditStatsItemForm::EditStatsItemForm(QWidget *parent,
        const std::string &resourcesPath,
        std::shared_ptr<ItemStore> itemStore,
        std::optional<std::string> itemIdToEdit)
    : EditItemFormBase(parent, resourcesPath, itemIdToEdit),
    ui(Ui::editStatsItemFormClass()),
    m_controller(itemStore)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/ItemEditor Icon.png"));
    connectUIActions();
    initializeComboBoxStatChanging();
    if (m_itemIdToEdit.has_value()) {
        this->setWindowTitle("Edit stats item");
        if (!loadExistingItemToForm()) {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
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

bool EditStatsItemForm::loadExistingItemToForm()
{
    auto existingItem = m_controller.getItem(*m_itemIdToEdit);
    if (existingItem != nullptr) {
        auto *statsItemDTO = dynamic_cast<StatsItemDTO *>(existingItem.get());
        if (statsItemDTO != nullptr) {
            ui.lineEditId->setText(statsItemDTO->id.c_str());
            ui.lineEditName->setText(statsItemDTO->name.c_str());
            ui.lineEditTextureName->setText(statsItemDTO->textureName.c_str());
            ui.spinBoxTextureIndex->setValue(statsItemDTO->textureIndex);
            ui.lineEditOptionalDescription->setText(statsItemDTO->optionalDescription.c_str());
            ui.comboBoxStatChanging->setCurrentIndex(statsItemDTO->statChangingIndex);
            ui.lineEditGain->setText(std::to_string(statsItemDTO->gain).c_str());
            ui.checkBoxLimitOfOneApplied->setChecked(statsItemDTO->limitOfOneApplied);
            ui.lineEditDurationInSecs->setText(std::to_string(statsItemDTO->durationInSecs).c_str());
        }
        else {
            ErrorMessage::show("Unable to cast the selected item to stats item type");
            return false;
        }
    }
    else {
        ErrorMessage::show("Unable to load the selected item");
        return false;
    }
    return true;
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

    auto itemInfo = std::make_unique<StatsItemDTO>();
    itemInfo->id = ui.lineEditId->text().toStdString();
    itemInfo->name = ui.lineEditName->text().toStdString();
    itemInfo->textureName = ui.lineEditTextureName->text().toStdString();
    itemInfo->textureIndex = ui.spinBoxTextureIndex->value();
    itemInfo->optionalDescription = ui.lineEditOptionalDescription->text().toStdString();
    itemInfo->statChangingIndex = ui.comboBoxStatChanging->currentIndex();
    itemInfo->gain = stof(ui.lineEditGain->text().toStdString());
    itemInfo->limitOfOneApplied = ui.checkBoxLimitOfOneApplied->isChecked();
    itemInfo->durationInSecs = static_cast<unsigned int>(std::stoul(ui.lineEditDurationInSecs->text().toStdString()));
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

void EditStatsItemForm::onPushButtonTexturePickerClick()
{
    auto result = showTexturePicker({ ui.lineEditTextureName->text().toStdString(),
            ui.spinBoxTextureIndex->value() },
            m_controller.getTextureContainer());
    if (result.has_value()) {
        ui.lineEditTextureName->setText(result->textureName.c_str());
        ui.spinBoxTextureIndex->setValue(result->textureIndex);
    }
}
