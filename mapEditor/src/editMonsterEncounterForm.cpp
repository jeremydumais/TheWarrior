#include "editMonsterEncounterForm.hpp"
#include "errorMessage.hpp"
#include "monsterEncounterDTO.hpp"
#include "monsterPickerForm.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <optional>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtimer.h>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;
using namespace boost::algorithm;

EditMonsterEncounterForm::EditMonsterEncounterForm(QWidget *parent,
                                                   const std::shared_ptr<ContainerOfMonsterStore> monsterStores,
                                                   const std::string &resourcesPath,
                                                   const std::vector<std::string> &alreadyUsedMonsterIds,
                                                   const std::optional<MonsterEncounterDTO> &editMonsterInfo)
    : QDialog(parent),
    ui(Ui::editMonsterEncounterFormClass()),
    m_controller(monsterStores, resourcesPath, alreadyUsedMonsterIds),
    m_editMonsterInfo(editMonsterInfo)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    populateComboBoxEncounter();
    connectUIActions();
    if (m_editMonsterInfo.has_value()) {
        this->setWindowTitle("Edit monster encounter");
        if (!loadExistingItemToForm()) {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
    else {
        this->setWindowTitle("Add monster encounter");
    }
}
mapeditor::controllers::MonsterEncounterDTO EditMonsterEncounterForm::getResult() const
{
    return MonsterEncounterDTO {
        .monsterId = trim_copy(ui.lineEditId->text().toStdString()),
        .monsterName = "",
        .encounterRatio = ui.comboBoxEncounter->itemText(ui.comboBoxEncounter->currentIndex()).toStdString(),
        .monsterIcon = std::nullopt
    };
}

void EditMonsterEncounterForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterEncounterForm::onPushButtonOKClick);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMonsterEncounterForm::reject);
    connect(ui.pushButtonMonsterSelector, &QPushButton::clicked, this, &EditMonsterEncounterForm::onPushButtonMonsterSelectorClick);
    connect(ui.lineEditId, &QLineEdit::editingFinished, this , &EditMonsterEncounterForm::onLineEditIdEditingFinish);
}

void EditMonsterEncounterForm::populateComboBoxEncounter()
{
    ui.comboBoxEncounter->model()->removeRows(0, ui.comboBoxEncounter->count());
    ui.comboBoxEncounter->insertItem(0, "Normal");
    ui.comboBoxEncounter->insertItem(1, "Less than normal");
    ui.comboBoxEncounter->insertItem(2, "Rare");
}

bool EditMonsterEncounterForm::loadExistingItemToForm()
{
    ui.lineEditId->setText(m_editMonsterInfo.value().monsterId.c_str());
    const auto ratioIndex = m_controller.getEncounterRatioIndex(m_editMonsterInfo.value().encounterRatio);
    if (ratioIndex == -1) {
        ErrorMessage::show("Unable to convert the encounter ratio value");
        return false;
    }
    else {
        ui.comboBoxEncounter->setCurrentIndex(ratioIndex);
    }
    onLineEditIdEditingFinish();
    return true;
}

void EditMonsterEncounterForm::onPushButtonOKClick()
{
    const auto monsterId = ui.lineEditId->text().toStdString();
    if (trim_copy(monsterId).empty()) {
        ErrorMessage::show("Monster Id cannot be empty");
        return;
    }
    if (!m_controller.isMonsterIdValid(monsterId)) {
        ErrorMessage::show(fmt::format("Monster Id {0} has an incorrect format.", monsterId));
        return;
    }
    if ((!m_editMonsterInfo.has_value() && m_controller.isMonsterAlreadyUsed(monsterId)) ||
        (m_editMonsterInfo.has_value() && m_controller.isMonsterAlreadyUsed(monsterId) && to_upper_copy(monsterId) != to_upper_copy(m_editMonsterInfo.value().monsterId))) {
        ErrorMessage::show(fmt::format("Monster {0} is already part of the zone.", monsterId));
        return;
    }
    accept();
}

void EditMonsterEncounterForm::onPushButtonMonsterSelectorClick()
{
    MonsterPickerForm pickerForm(this,
                                 m_controller.getMonsterStores(),
                                 m_controller.getResourcesPath());
    if (pickerForm.exec() == QDialog::Accepted) {
        ui.lineEditId->setText(pickerForm.getSelectedMonsterId().c_str());
        onLineEditIdEditingFinish();
    }
}

void EditMonsterEncounterForm::onLineEditIdEditingFinish()
{
    const auto id = ui.lineEditId->text().toStdString();
    ui.labelMonsterName->setText(m_controller.getMonsterNameById(id).c_str());
    //Try to load the monster image
    const auto icon = m_controller.getMonsterIcon(id);
    if (icon.has_value()) {
        ui.labelMonsterImage->setPixmap(icon->pixmap(64, 64));
    }
    else {
        ui.labelMonsterImage->clear();
    }
}
