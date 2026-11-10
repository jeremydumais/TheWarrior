#ifndef MONSTEREDITOR_SRC_EDITMONSTERFORM_HPP_
#define MONSTEREDITOR_SRC_EDITMONSTERFORM_HPP_

#include <memory>
#include <optional>
#include <string>
#include "ui_editMonsterForm.h"
#include "manageMonsterController.hpp"
#include "monsterStore.hpp"

class EditMonsterForm : public QDialog {
Q_OBJECT

 public:
    EditMonsterForm(QWidget *parent,
            const std::string &resourcesPath,
            std::shared_ptr<thewarrior::models::MonsterStore> monsterStore,
            std::optional<std::string> monsterIdToEdit);
    void connectUIActions();

 private:
    Ui::editMonsterFormClass ui;
    monstereditor::controllers::ManageMonsterController m_controller;
    std::string m_resourcesPath;
    std::optional<std::string> m_monsterIdToEdit;
    bool loadExistingMonsterToForm();
    void onPushButtonCancelClick();
    void onPushButtonOKClick();
    void onPushButtonTexturePickerClick();
    void refreshSelectedTexture();
    void onPushButtonOpenMusicFileClick();
    void onPushButtonClearMusicClick();
};

#endif  // MONSTEREDITOR_SRC_EDITMONSTERFORM_HPP_
