
#ifndef MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_
#define MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_

#include <qabstractitemmodel.h>
#include <qchar.h>
#include <qdialog.h>
#include <string>
#include <vector>
#include "fileTexturePixmapProvider.hpp"
#include "texture.hpp"
#include "ui_selectNPCTextureForm.h"
#include "selectNPCTextureFormController.hpp"

class SelectNPCTextureForm : public QDialog {
Q_OBJECT

 public:
    SelectNPCTextureForm(QWidget *parent,
                         const std::string &resourcesPath,
                         const std::vector<thewarrior::models::Texture> &textures);
 private:
    Ui::selectNPCTextureFormClass ui;
    commoneditor::ui::FileTexturePixmapProvider m_texturePixmapProvider;
    mapeditor::controllers::SelectNPCTextureFormController m_controller;
    void connectUIActions();
    void refreshPositionLabel();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onComboBoxTextureNameChanged(int index);
    void onListViewNPCAvailableCurrentChanged(const QModelIndex &current,
                                              const QModelIndex &previous);
};

#endif  // MAPEDITOR_SRC_SELECTNPCTEXTUREFORM_HPP_
