#include <fmt/core.h>
#include <fmt/format.h>
#include <string>
#include <vector>
#include "editNPCForm.hpp"
#include "editNPCFormController.hpp"
#include "errorMessage.hpp"
#include "npc.hpp"
#include "npcDTO.hpp"
#include "point.hpp"
#include "selectNPCTextureForm.hpp"
#include "texture.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditNPCFormController;
using mapeditor::controllers::NPCDTO;
using thewarrior::models::NPCFacing;
using thewarrior::models::Point;
using thewarrior::models::Texture;

EditNPCForm::EditNPCForm(QWidget *parent,
                         const std::string &resourcesPath,
                         const std::vector<Texture> &textures,
                         const std::optional<NPCDTO> selectedNPC,
                         const std::vector<std::string> &alreadyUsedNPCIds)
    : QDialog(parent),
    ui(Ui::editNPCFormClass()),
    m_controller(resourcesPath, textures, m_texturePixmapProvider, selectedNPC, alreadyUsedNPCIds) {
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

bool EditNPCForm::isEditMode() const {
    return m_controller.isEditMode();
}

bool EditNPCForm::isSpawnPositionPickerModeEnabled() const {
    return m_spawnPositionPickerModeEnabled;
}

void EditNPCForm::restoreFromPicker(const Point<> &position) {
    m_spawnPositionPickerModeEnabled = false;
    m_spawnPosition = position;
    refreshPositionLabel();
}

const NPCDTO &EditNPCForm::getResult() const {
    return m_result;
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
    if (m_result.textureName.empty() || m_result.baseTextureIndex == -1) {
        ErrorMessage::show("The NPC texture is required.");
        return;
    }
    if (m_spawnPosition == Point<>(-1, -1)) {
        ErrorMessage::show("The spawn position is required.");
        return;
    }
    const auto npcId = ui.lineEditId->text().trimmed().toStdString();
    if (m_controller.isNPCIdAlreadyUsed(npcId)) {
        ErrorMessage::show(fmt::format("The NPC Id {} already exists in the list.", npcId));
        return;
    }
    m_result.id = npcId;
    m_result.name = ui.lineEditName->text().trimmed().toStdString();
    m_result.spawnPosition = Point<size_t>(static_cast<size_t>(m_spawnPosition.x()),
                                          static_cast<size_t>(m_spawnPosition.y()));
    m_result.dialogueLines = EditNPCFormController::convertPlainTextToLines(ui.plainTextDialogue->toPlainText());
    m_result.defaultFacing = static_cast<NPCFacing>(ui.comboBoxDefaultFacing->currentIndex());
    m_result.currentFacing = m_result.defaultFacing;
    if (!m_controller.isDTOValid(m_result)) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
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
