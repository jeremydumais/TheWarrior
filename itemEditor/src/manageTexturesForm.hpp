#ifndef MANAGETEXTURESFORM_H
#define MANAGETEXTURESFORM_H

#include "manageTexturesController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_manageTexturesForm.h"
#include <memory>
#include <string>

class ManageTexturesForm : public QDialog
{
Q_OBJECT

public:
    ManageTexturesForm(QWidget *parent,
            const std::string &resourcesPath,
            thewarrior::models::TextureContainer &textureContainer);
    void connectUIActions();
private:
    Ui::manageTexturesFormClass ui;
    const std::string m_resourcesPath;
    itemeditor::controllers::ManageTextureController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetTexturesKeyWatcher;
    void refreshTextureList();
    void onPushButtonCloseClick();
    std::unique_ptr<itemeditor::controllers::TextureDTO> getSelectedTextureInTextureList();
    void onPushButtonAddClick();
    void onPushButtonEditClick();
    void onPushButtonDeleteClick();
    void onTableWidgetTexturesDoubleClicked(QTableWidgetItem *item);
    void onTableWidgetTexturesKeyPressEvent(int key, int, int);
};

#endif // MANAGETEXTURESFORM_H
