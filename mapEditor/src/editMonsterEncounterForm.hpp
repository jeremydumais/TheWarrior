#ifndef EDITMONSTERENCOUNTERFORM_H
#define EDITMONSTERENCOUNTERFORM_H

#include "ui_editMonsterEncounterForm.h"
#include <string>

class EditMonsterEncounterForm : public QDialog
{
Q_OBJECT

public:
    explicit EditMonsterEncounterForm(QWidget *parent);
private:
    Ui::editMonsterEncounterFormClass ui;
    void connectUIActions();
};

#endif // EDITMONSTERENCOUNTERFORM_H
