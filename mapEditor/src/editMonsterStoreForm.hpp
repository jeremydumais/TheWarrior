
#ifndef EDITMONSTERSTOREFORM_H
#define EDITMONSTERSTOREFORM_H

#include "ui_editMonsterStoreForm.h"
#include "manageMonsterStoreController.hpp"
#include "monsterStoreInfo.hpp"
#include <functional>
#include <optional>
#include <qdialog.h>
#include <string>
#include <vector>

class EditMonsterStoreForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterStoreForm(QWidget *parent,
                               mapeditor::controllers::ManageMonsterStoreController &controller,
                               std::optional<std::reference_wrapper<const std::string>> nameToEdit);
private:
    Ui::editMonsterStoreFormClass ui;
    mapeditor::controllers::ManageMonsterStoreController &m_controller;
	bool m_isEditMode;
    const std::string m_monsterStoreNameToEdit;
    void connectUIActions();
    bool loadExistingMonsterStoreToForm();
    void onPushButtonOKClick();
    void onPushButtonOpenFileClick();
};

#endif // EDITMONSTERSTOREFORM_H
