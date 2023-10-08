#ifndef MONSTEREDITOR_SRC_MANAGETEXTURESFORM_HPP_
#define MONSTEREDITOR_SRC_MANAGETEXTURESFORM_HPP_

#include <memory>
#include <string>
#include "manageTexturesController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_manageTexturesForm.h"

class ManageTexturesForm : public QDialog {
Q_OBJECT

 public:
    ManageTexturesForm(QWidget *parent,
            const std::string &resourcesPath,
            thewarrior::models::TextureContainer &textureContainer);
    void connectUIActions();

 private:
    Ui::manageTexturesFormClass ui;
    const std::string m_resourcesPath;
    monstereditor::controllers::ManageTextureController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetTexturesKeyWatcher;
    void refreshTextureList();
    void onPushButtonCloseClick();
    std::unique_ptr<monstereditor::controllers::TextureDTO> getSelectedTextureInTextureList();
    void onPushButtonAddClick();
    void onPushButtonEditClick();
    void onPushButtonDeleteClick();
    void onTableWidgetTexturesDoubleClicked(QTableWidgetItem *item);
    void onTableWidgetTexturesKeyPressEvent(int key, int, int);
};

#endif  // MONSTEREDITOR_SRC_MANAGETEXTURESFORM_HPP_
