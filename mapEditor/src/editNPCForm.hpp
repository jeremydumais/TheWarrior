
#ifndef MAPEDITOR_SRC_EDITNPCFORM_HPP_
#define MAPEDITOR_SRC_EDITNPCFORM_HPP_

#include <qdialog.h>
#include <optional>
#include <string>
#include <vector>
#include "fileTexturePixmapProvider.hpp"
#include "glComponentController.hpp"
#include "npcDTO.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "ui_editNPCForm.h"
#include "editNPCFormController.hpp"
#include "conversationScenario.hpp"
#include "qTableWidgetKeyPressWatcher.h"

class EditNPCForm : public QDialog {
Q_OBJECT

 public:
    EditNPCForm(QWidget *parent,
                const mapeditor::controllers::GLComponentController *glComponentController,
                const std::string &resourcesPath,
                const std::vector<thewarrior::models::Texture> &textures,
                const std::optional<mapeditor::controllers::NPCDTO> &selectedNPC,
                const std::vector<std::string> &alreadyUsedNPCIds);
    bool isEditMode() const;
    bool isSpawnPositionPickerModeEnabled() const;
    void restoreFromPicker(const thewarrior::models::Point<> &position);
    const mapeditor::controllers::NPCDTO &getResult() const;

 private:
    Ui::editNPCFormClass ui;
    mapeditor::controllers::EditNPCFormController m_controller;
    commoneditor::ui::FileTexturePixmapProvider m_texturePixmapProvider;
    bool m_spawnPositionPickerModeEnabled = false;
    thewarrior::models::Point<> m_spawnPosition = thewarrior::models::Point<>(-1, -1);
    mapeditor::controllers::NPCDTO m_result;
    QTableWidgetKeyPressWatcher tableWidgetConvScenarioKeyWatcher;

    void initializeConversationScenariosTable();
    void connectUIActions();
    void refreshPositionLabel();
    void refreshNPCTile();
    void refreshConversationScenarioList();
    void setConditionalVisibilityControlsEnabled(bool enabled);
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonSelectTextureClick();
    void onPushButtonSpawnPositionPickerClick();
    void onPushButtonAddConvScenarioClick();
    void onPushButtonEditConvScenarioClick();
    void onPushButtonDeleteConvScenarioClick();
    void onTableWidgetConvScenarioDoubleClicked(QTableWidgetItem *item);
    void onTableWidgetConvScenarioKeyPressEvent(int key, int row, int column);
    std::optional<thewarrior::models::ConversationScenarioId> getSelectedScenarioId() const;
};

#endif  // MAPEDITOR_SRC_EDITNPCFORM_HPP_
