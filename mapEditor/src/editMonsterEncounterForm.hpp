#ifndef EDITMONSTERENCOUNTERFORM_H
#define EDITMONSTERENCOUNTERFORM_H

#include "ui_editMonsterEncounterForm.h"
#include "editMonsterEncounterFormController.hpp"
#include "types.hpp"
#include <memory>
#include <string>

class EditMonsterEncounterForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterEncounterForm(QWidget *parent,
                                      const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
private:
    Ui::editMonsterEncounterFormClass ui;
    mapeditor::controllers::EditMonsterEncounterFormController m_controller;
    void connectUIActions();
    void onPushButtonMonsterSelectorClick();
};

#endif // EDITMONSTERENCOUNTERFORM_H
