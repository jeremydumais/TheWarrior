#include <fmt/core.h>
#include <fmt/format.h>
#include <string>
#include <vector>
#include "editNPCForm.hpp"
#include "editNPCFormController.hpp"
#include "errorMessage.hpp"
#include "point.hpp"
#include "selectNPCTextureForm.hpp"

using commoneditor::ui::ErrorMessage;
using thewarrior::models::Point;

EditNPCForm::EditNPCForm(QWidget *parent,
                         const std::string &resourcesPath,
                         const std::vector<thewarrior::models::Texture> &textures)
    : QDialog(parent),
    ui(Ui::editNPCFormClass()),
    m_controller(resourcesPath, textures, m_texturePixmapProvider) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    ui.comboBoxDefaultFacing->addItem("Left");
    ui.comboBoxDefaultFacing->addItem("Up");
    ui.comboBoxDefaultFacing->addItem("Right");
    ui.comboBoxDefaultFacing->addItem("Down");
    ui.comboBoxDefaultFacing->setCurrentIndex(3);
    connectUIActions();
    refreshPositionLabel();
}

bool EditNPCForm::isSpawnPositionPickerModeEnabled() const {
    return m_spawnPositionPickerModeEnabled;
}

void EditNPCForm::restoreFromPicker(const thewarrior::models::Point<> &position) {
    m_spawnPositionPickerModeEnabled = false;
    m_spawnPosition = position;
    refreshPositionLabel();
}

void EditNPCForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditNPCForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditNPCForm::onPushButtonOKClick);
    connect(ui.pushButtonSelectTexture, &QPushButton::clicked, this, &EditNPCForm::onPushButtonSelectTextureClick);
    connect(ui.pushButtonSpawnPositionPicker, &QPushButton::clicked, this, &EditNPCForm::onPushButtonSpawnPositionPickerClick);
}

void EditNPCForm::refreshPositionLabel() {
    if (m_spawnPosition.x() < 0 || m_spawnPosition.y() < 0) {
        ui.labelSpawnPositionValue->setText("<no value selected>");
    } else {
        ui.labelSpawnPositionValue->setText(fmt::format("({0}, {1})",
                                                        m_spawnPosition.x(),
                                                        m_spawnPosition.y()).c_str());
    }
}

void EditNPCForm::onPushButtonCancelClick() {
    reject();
}

void EditNPCForm::onPushButtonOKClick() {
    if (ui.lineEditId->text().trimmed().isEmpty()) {
        ErrorMessage::show("The id is required.");
        return;
    }
    if (ui.lineEditName->text().trimmed().isEmpty()) {
        ErrorMessage::show("The name is required.");
        return;
    }
    if (m_spawnPosition == Point<>(-1, -1)) {
        ErrorMessage::show("The spawn position is required.");
        return;
    }

    //TODO: 0.6.0 Ensure the ID is unique
    accept();
}

void EditNPCForm::onPushButtonSelectTextureClick() {
    SelectNPCTextureForm formSelectTexture(this,
                                           m_controller.getResourcesPath(),
                                           m_controller.getTextures());
    if (formSelectTexture.exec() == QDialog::Accepted) {
        const auto textureResult = formSelectTexture.getResult();
        m_result.textureName = textureResult.textureName;
        m_result.baseTextureIndex = textureResult.baseTextureIndex;
        const auto npcTextureResult = m_controller.getNPCPixmap(m_result.textureName,
                                                                m_result.baseTextureIndex);
        if (npcTextureResult.success) {
            ui.labelNPCTexture->setPixmap(*npcTextureResult.result);
        } else {
            ui.labelNPCTexture->clear();
            ErrorMessage::show(fmt::format("Unable to load the NPC image. {0}",
                                           m_controller.getLastError()));
        }
    }
}

void EditNPCForm::onPushButtonSpawnPositionPickerClick() {
    m_spawnPositionPickerModeEnabled = true;
    accept();
}
