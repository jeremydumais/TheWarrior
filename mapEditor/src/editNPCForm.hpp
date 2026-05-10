
#ifndef MAPEDITOR_SRC_EDITNPCFORM_HPP_
#define MAPEDITOR_SRC_EDITNPCFORM_HPP_

#include <qdialog.h>
#include <string>
#include <vector>
#include "fileTexturePixmapProvider.hpp"
#include "npcDTO.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "ui_editNPCForm.h"
#include "editNPCFormController.hpp"

class EditNPCForm : public QDialog {
Q_OBJECT

 public:
    EditNPCForm(QWidget *parent,
                const std::string &resourcesPath,
                const std::vector<thewarrior::models::Texture> &textures,
                const std::optional<mapeditor::controllers::NPCDTO> selectedNPC,
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
    void connectUIActions();
    void refreshPositionLabel();
    void refreshNPCTile();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonSelectTextureClick();
    void onPushButtonSpawnPositionPickerClick();
};

#endif  // MAPEDITOR_SRC_EDITNPCFORM_HPP_
