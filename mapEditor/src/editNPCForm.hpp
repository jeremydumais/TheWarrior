
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
                const std::vector<thewarrior::models::Texture> &textures);
    bool isSpawnPositionPickerModeEnabled() const;
    void restoreFromPicker(const thewarrior::models::Point<> &position);
 private:
    Ui::editNPCFormClass ui;
    mapeditor::controllers::EditNPCFormController m_controller;
    commoneditor::ui::FileTexturePixmapProvider m_texturePixmapProvider;
    bool m_spawnPositionPickerModeEnabled = false;
    thewarrior::models::Point<> m_spawnPosition = thewarrior::models::Point<>(-1, -1);
    mapeditor::controllers::NPCDTO m_result;
    void connectUIActions();
    void refreshPositionLabel();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonSelectTextureClick();
    void onPushButtonSpawnPositionPickerClick();
};

#endif  // MAPEDITOR_SRC_EDITNPCFORM_HPP_
