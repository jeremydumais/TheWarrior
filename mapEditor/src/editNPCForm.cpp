#include <fmt/core.h>
#include <fmt/format.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationScenarioForm.hpp"
#include "editNPCForm.hpp"
#include "editNPCFormController.hpp"
#include "errorMessage.hpp"
#include "glComponentController.hpp"
#include "npc.hpp"
#include "npcDTO.hpp"
#include "point.hpp"
#include "selectNPCTextureForm.hpp"
#include "texture.hpp"
#include "warningMessage.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::WarningMessage;
using mapeditor::controllers::EditNPCFormController;
using mapeditor::controllers::NPCDTO;
using thewarrior::models::ConversationScenarioId;
using thewarrior::models::NPCBehavior;
using thewarrior::models::NPCFacing;
using thewarrior::models::NPCVisibilityCondition;
using thewarrior::models::NPCVisibilityRule;
using thewarrior::models::Point;
using thewarrior::models::Texture;

EditNPCForm::EditNPCForm(QWidget *parent,
                         const mapeditor::controllers::GLComponentController *glComponentController,
                         const std::string &resourcesPath,
                         const std::vector<Texture> &textures,
                         const std::optional<NPCDTO> &selectedNPC,
                         const std::vector<std::string> &alreadyUsedNPCIds)
    : QDialog(parent),
    ui(Ui::editNPCFormClass()),
    m_controller(glComponentController, resourcesPath, textures, m_texturePixmapProvider, selectedNPC, alreadyUsedNPCIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    ui.comboBoxDefaultFacing->addItem("Left");
    ui.comboBoxDefaultFacing->addItem("Up");
    ui.comboBoxDefaultFacing->addItem("Right");
    ui.comboBoxDefaultFacing->addItem("Down");
    ui.comboBoxDefaultFacing->setCurrentIndex(3);
    ui.comboBoxDefaultBehavior->addItem("Stationary");
    ui.comboBoxDefaultBehavior->addItem("Wander");
    ui.comboBoxDefaultBehavior->setCurrentIndex(0);
    ui.comboBoxVisibilityCondition->addItem("Any completed");
    ui.comboBoxVisibilityCondition->addItem("None completed");
    ui.comboBoxVisibilityCondition->setCurrentIndex(-1);
    if (selectedNPC.has_value()) {
        m_result = selectedNPC.value();
        ui.lineEditId->setText(selectedNPC->id.c_str());
        ui.lineEditName->setText(selectedNPC->name.c_str());
        m_spawnPosition = Point<>(static_cast<int>(selectedNPC->spawnPosition.x()),
                                  static_cast<int>(selectedNPC->spawnPosition.y()));
        m_result.textureName = selectedNPC->textureName;
        m_result.baseTextureIndex = selectedNPC->baseTextureIndex;
        ui.comboBoxDefaultFacing->setCurrentIndex(static_cast<int>(selectedNPC->defaultFacing));
        ui.comboBoxDefaultBehavior->setCurrentIndex(static_cast<int>(selectedNPC->defaultBehavior));
        if (selectedNPC->visibilityRule.has_value()) {
            ui.checkBoxEnableConditionalVisibility->setChecked(true);
            ui.comboBoxVisibilityCondition->setCurrentIndex(static_cast<int>(selectedNPC->visibilityRule->condition));
            ui.lineEditVisibilityConditionStoryIds->setText(m_controller.getVisibilityConditionStoryIdsText().c_str());
        }
        refreshNPCTile();
        refreshConversationScenarioList();
    }
    initializeConversationScenariosTable();
    connectUIActions();
    setConditionalVisibilityControlsEnabled(ui.checkBoxEnableConditionalVisibility->isChecked());
    refreshPositionLabel();

}
bool EditNPCForm::isEditMode() const {
    return m_controller.isEditMode();
}

bool EditNPCForm::isSpawnPositionPickerModeEnabled() const {
    return m_spawnPositionPickerModeEnabled;
}

void EditNPCForm::refreshConversationScenarioList() {
    ui.tableWidgetConvScenarios->model()->removeRows(0, ui.tableWidgetConvScenarios->rowCount());
    int index = 0;
    for (const auto &dto : m_controller.getConversationScenariosDTO()) {
        auto *nodeCountColumn = new QTableWidgetItem(std::to_string(dto.nodeCount).c_str());
        nodeCountColumn->setTextAlignment(Qt::AlignRight);
        ui.tableWidgetConvScenarios->insertRow(index);
        ui.tableWidgetConvScenarios->setItem(index, 0, new QTableWidgetItem(dto.id.c_str()));
        ui.tableWidgetConvScenarios->setItem(index, 1, nodeCountColumn);
        index++;
    }
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
    connect(ui.pushButtonAddConvScenario, &QPushButton::clicked, this, &EditNPCForm::onPushButtonAddConvScenarioClick);
    connect(ui.pushButtonEditConvScenario, &QPushButton::clicked, this, &EditNPCForm::onPushButtonEditConvScenarioClick);
    connect(ui.tableWidgetConvScenarios, &QTableWidget::itemDoubleClicked, this, &EditNPCForm::onTableWidgetConvScenarioDoubleClicked);
    connect(ui.pushButtonDeleteConvScenario, &QPushButton::clicked, this, &EditNPCForm::onPushButtonDeleteConvScenarioClick);
    connect(ui.checkBoxEnableConditionalVisibility, &QCheckBox::toggled,
            this, &EditNPCForm::setConditionalVisibilityControlsEnabled);
    tableWidgetConvScenarioKeyWatcher.installOn(ui.tableWidgetConvScenarios);
    connect(&tableWidgetConvScenarioKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &EditNPCForm::onTableWidgetConvScenarioKeyPressEvent);
}

void EditNPCForm::setConditionalVisibilityControlsEnabled(bool enabled) {
    ui.comboBoxVisibilityCondition->setEnabled(enabled);
    ui.lineEditVisibilityConditionStoryIds->setEnabled(enabled);
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

void EditNPCForm::refreshNPCTile() {
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

void EditNPCForm::initializeConversationScenariosTable() {
    ui.tableWidgetConvScenarios->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetConvScenarios->setHorizontalHeaderItem(1, new QTableWidgetItem("Item Count"));
    ui.tableWidgetConvScenarios->setColumnWidth(0, 395);
    ui.tableWidgetConvScenarios->setColumnWidth(1, 60);
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
    if (ui.tableWidgetConvScenarios->rowCount() == 0) {
        WarningMessage::show("Since the conversation scenarion list is empty, nothing will happen if you try to talk to the NPC.");
    }
    if (ui.checkBoxEnableConditionalVisibility->isChecked()) {
        if (ui.comboBoxVisibilityCondition->currentIndex() == -1) {
            ErrorMessage::show("You must select a visibility condition.");
            ui.comboBoxVisibilityCondition->setFocus();
            return;
        }
        if (ui.lineEditVisibilityConditionStoryIds->text().trimmed().isEmpty()) {
            ErrorMessage::show("You must enter at least one story ID to apply conditional visibility.");
            ui.lineEditVisibilityConditionStoryIds->setFocus();
            return; 
        }
    }

    m_result.id = npcId;
    m_result.name = ui.lineEditName->text().trimmed().toStdString();
    m_result.spawnPosition = Point<size_t>(static_cast<size_t>(m_spawnPosition.x()),
                                          static_cast<size_t>(m_spawnPosition.y()));
    if (m_controller.isEditMode()) {
        const auto selectedNPC = m_controller.getSelectedNPC();
        m_result.wanderZone = selectedNPC ? selectedNPC->wanderZone : decltype(selectedNPC->wanderZone){};
    }
    m_result.conversationScenarios = m_controller.getConversationScenarios();
    m_result.defaultFacing = static_cast<NPCFacing>(ui.comboBoxDefaultFacing->currentIndex());
    m_result.currentFacing = m_result.defaultFacing;
    m_result.defaultBehavior = static_cast<NPCBehavior>(ui.comboBoxDefaultBehavior->currentIndex());
    m_result.currentBehavior = m_result.defaultBehavior;
    if (ui.checkBoxEnableConditionalVisibility->isChecked()) {
        m_result.visibilityRule = NPCVisibilityRule {
            .condition = static_cast<NPCVisibilityCondition>(ui.comboBoxVisibilityCondition->currentIndex()),
            .storyIds = m_controller.splitVisibilityConditionStoryIds(ui.lineEditVisibilityConditionStoryIds->text())
        };
    } else {
        m_result.visibilityRule = std::nullopt;
    }
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
        refreshNPCTile();
    }
}

void EditNPCForm::onPushButtonSpawnPositionPickerClick() {
    m_spawnPositionPickerModeEnabled = true;
    accept();
}

void EditNPCForm::onPushButtonAddConvScenarioClick() {
    auto existingIds = m_controller.getAlreadyUsedScenarioIds();
    EditConversationScenarioForm conversationScenarioForm(this,
                                                          m_controller.getGLComponentController(),
                                                          m_controller.getResourcesPath(),
                                                          std::nullopt,
                                                          existingIds);
    if (conversationScenarioForm.exec() == QDialog::Accepted) {
        m_controller.addConversationScenario(conversationScenarioForm.getResult());
        refreshConversationScenarioList();
    }
}

void EditNPCForm::onPushButtonEditConvScenarioClick() {
    
    if (auto scenarioId = getSelectedScenarioId(); scenarioId.has_value()) {
        auto itemToEdit = m_controller.getConversationScenarioById(scenarioId.value());
        if (!itemToEdit.has_value()) {
            ErrorMessage::show(fmt::format("Unable to find the scenario with id {0}", scenarioId.value()));
            return;
        }
        auto existingIds = m_controller.getAlreadyUsedScenarioIds();
        EditConversationScenarioForm conversationScenarioForm(this,
                                                              m_controller.getGLComponentController(),
                                                              m_controller.getResourcesPath(),
                                                              itemToEdit,
                                                              existingIds);
        if (conversationScenarioForm.exec() == QDialog::Accepted) {
            if (!m_controller.updateConversationScenario(itemToEdit->getId(), conversationScenarioForm.getResult())) {
                ErrorMessage::show(m_controller.getLastError());
            }
            refreshConversationScenarioList();
        }
    }
}

void EditNPCForm::onPushButtonDeleteConvScenarioClick() {
    if (auto scenarioId = getSelectedScenarioId(); scenarioId.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the conversation scenario {}?", scenarioId.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (!m_controller.removeConversationScenario(scenarioId.value())) {
                ErrorMessage::show(m_controller.getLastError());
            }
            refreshConversationScenarioList();
        }
    }
}

void EditNPCForm::onTableWidgetConvScenarioDoubleClicked(QTableWidgetItem *item) {
    if (item != nullptr) {
        onPushButtonEditConvScenarioClick();
    }
}

void EditNPCForm::onTableWidgetConvScenarioKeyPressEvent(int key, int /*row*/, int /*column*/) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteConvScenarioClick();
    }
}

std::optional<ConversationScenarioId> EditNPCForm::getSelectedScenarioId() const {
    auto selectedRows = ui.tableWidgetConvScenarios->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}
