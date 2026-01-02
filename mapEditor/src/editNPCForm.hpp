
#ifndef MAPEDITOR_SRC_EDITNPCFORM_HPP_
#define MAPEDITOR_SRC_EDITNPCFORM_HPP_

#include <qdialog.h>
#include <string>
#include "point.hpp"
#include "ui_editNPCForm.h"
#include "editNPCFormController.hpp"

class EditNPCForm : public QDialog {
Q_OBJECT

 public:
    EditNPCForm(QWidget *parent,
                const std::string &resourcesPath);
    bool isSpawnPositionPickerModeEnabled() const;
    void restoreFromPicker(const thewarrior::models::Point<> &position);
 private:
    Ui::editNPCFormClass ui;
    mapeditor::controllers::EditNPCFormController m_controller;
    bool m_spawnPositionPickerModeEnabled = false;
    thewarrior::models::Point<> m_spawnPosition = thewarrior::models::Point<>(-1, -1);
    void connectUIActions();
    void refreshPositionLabel();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonSpawnPositionPickerClick();
};

#endif  // MAPEDITOR_SRC_EDITNPCFORM_HPP_
