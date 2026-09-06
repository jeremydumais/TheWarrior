
#ifndef MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_
#define MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_

#include <qabstractitemmodel.h>
#include <qchar.h>
#include <qdialog.h>
#include <qtimer.h>
#include <string>
#include <vector>
#include "fileTexturePixmapProvider.hpp"
#include "npc.hpp"
#include "texture.hpp"
#include "ui_selectNPCTextureForm.h"
#include "selectNPCTextureFormController.hpp"

class SelectNPCTextureForm : public QDialog {
Q_OBJECT

 public:
    struct SelectNPCTextureResult {
        std::string textureName = "";
        int baseTextureIndex = -1;
    };

    SelectNPCTextureForm(QWidget *parent,
                         const std::string &resourcesPath,
                         const std::vector<thewarrior::models::Texture> &textures,
                         thewarrior::models::NPCSpriteLayout spriteLayout);
    SelectNPCTextureResult getResult() const;

 private:
    Ui::selectNPCTextureFormClass ui;
    commoneditor::ui::FileTexturePixmapProvider m_texturePixmapProvider;
    mapeditor::controllers::SelectNPCTextureFormController m_controller;
    QTimer m_timerAnimateNPC;
    mapeditor::controllers::SelectNPCTextureFormController::NPCAnimation m_animationNPC;
    size_t m_animationNPCIndex = 0;
    bool m_animationDecrease = false;
    SelectNPCTextureResult m_result;
    void connectUIActions();
    void refreshPositionLabel();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onComboBoxTextureNameChanged(int index);
    void onListViewNPCAvailableCurrentChanged(const QModelIndex &current,
                                              const QModelIndex &previous);
    void ontimerAnimateNPCTimeout();
};

#endif  // MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_
