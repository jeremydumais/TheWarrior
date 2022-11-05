#include "editMonsterEncounterForm.hpp"
#include "monsterPickerForm.hpp"

using namespace mapeditor::controllers;

EditMonsterEncounterForm::EditMonsterEncounterForm(QWidget *parent,
                                                   const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : QDialog(parent),
    ui(Ui::editMonsterEncounterFormClass()),
    m_controller(monsterStores)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    onPushButtonMonsterSelectorClick();
}

void EditMonsterEncounterForm::connectUIActions()
{
    connect(ui.pushButtonMonsterSelector, &QPushButton::clicked, this, &EditMonsterEncounterForm::onPushButtonMonsterSelectorClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, std::terminate);//&EditMonsterEncounterForm::reject);
}

void EditMonsterEncounterForm::onPushButtonMonsterSelectorClick()
{
    MonsterPickerForm pickerForm(this, m_controller.getMonsterStores());
    pickerForm.exec();
}
