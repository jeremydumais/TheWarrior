#include "editMonsterEncounterForm.hpp"

EditMonsterEncounterForm::EditMonsterEncounterForm(QWidget *parent)
    : QDialog(parent),
    ui(Ui::editMonsterEncounterFormClass())
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
}

void EditMonsterEncounterForm::connectUIActions()
{

}
