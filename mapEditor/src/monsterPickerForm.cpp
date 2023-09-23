#include "monsterPickerForm.hpp"
#include "types.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <fmt/format.h>
#include <QString>
#include <algorithm>
#include <memory>

using namespace mapeditor::controllers;
using namespace thewarrior::models;
using namespace boost::algorithm;

MonsterPickerForm::MonsterPickerForm(QWidget *parent,
                                     const std::shared_ptr<ContainerOfMonsterStore> monsterStores,
                                     const std::string &resourcesPath)
    : QDialog(parent),
    ui(Ui::monsterPickerFormClass()),
    m_controller(monsterStores, resourcesPath)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    connectUIActions();
    populateMonsterStoreComboBox();
    initializeMonsterTable();
    refreshMonsterTable();
}

void MonsterPickerForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &MonsterPickerForm::onPushButtonOkClicked);//&EditMonsterEncounterForm::reject);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &MonsterPickerForm::reject);
    connect(ui.lineEditSearch, &QLineEdit::textChanged, this, &MonsterPickerForm::onLineEditSearchTextChanged);
    connect(ui.comboBoxStore, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MonsterPickerForm::onComboBoxStoreIndexChanged);
    connect(ui.tableWidgetMonsters, &QTableWidget::itemDoubleClicked, this, &MonsterPickerForm::onPushButtonOkClicked);
}

void MonsterPickerForm::populateMonsterStoreComboBox()
{
    ui.comboBoxStore->model()->removeRows(0, ui.comboBoxStore->count());
    for(const auto &storeName : m_controller.getMonsterStoreNames()) {
        ui.comboBoxStore->addItem(storeName.c_str());
    }
}

void MonsterPickerForm::initializeMonsterTable()
{
    ui.tableWidgetMonsters->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(2, new QTableWidgetItem("Health"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(3, new QTableWidgetItem("Attack"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(4, new QTableWidgetItem("Defense"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(5, new QTableWidgetItem("Gold reward"));
    ui.tableWidgetMonsters->setColumnWidth(0, 80);
    ui.tableWidgetMonsters->setColumnWidth(1, 200);
    ui.tableWidgetMonsters->setColumnWidth(2, 90);
    ui.tableWidgetMonsters->setColumnWidth(3, 90);
    ui.tableWidgetMonsters->setColumnWidth(4, 90);
    ui.tableWidgetMonsters->setColumnWidth(5, 90);
}

void MonsterPickerForm::refreshMonsterTable(const std::string &filter)
{
    int index = 0;
    ui.tableWidgetMonsters->model()->removeRows(0, ui.tableWidgetMonsters->rowCount());
    const auto selectedStoreName = ui.comboBoxStore->currentText().toStdString();
    const auto &monsterIds = m_controller.getMonsterIds(selectedStoreName, filter);
    auto monstersToDisplay = m_controller.getMonsters(selectedStoreName, monsterIds);
    auto monsterIdsWithIcon = m_controller.getMonstersIcon(selectedStoreName, monsterIds);
    for (const auto &monster : monstersToDisplay) {;
        ui.tableWidgetMonsters->insertRow(index);
        auto idMonster = new QTableWidgetItem(monster.id.c_str());
        if (monsterIdsWithIcon.find(monster.id) != monsterIdsWithIcon.end()) {
            idMonster->setIcon(monsterIdsWithIcon[monster.id]);
        }
        ui.tableWidgetMonsters->setItem(index, 0, idMonster);
        ui.tableWidgetMonsters->setItem(index, 1, new QTableWidgetItem(monster.name.c_str()));
        ui.tableWidgetMonsters->setItem(index, 2, new QTableWidgetItem(fmt::format("{}", monster.health).c_str()));
        ui.tableWidgetMonsters->setItem(index, 3, new QTableWidgetItem(fmt::format("{}", monster.attack).c_str()));
        ui.tableWidgetMonsters->setItem(index, 4, new QTableWidgetItem(fmt::format("{}", monster.defense).c_str()));
        ui.tableWidgetMonsters->setItem(index, 5, new QTableWidgetItem(fmt::format("[{}-{}]",
                                                                                   monster.goldMinimum,
                                                                                   monster.goldMaximum).c_str()));
        index++;
    }
}

const std::string &MonsterPickerForm::getSelectedMonsterId() const
{
    return m_selectedMonsterId;
}

void MonsterPickerForm::onPushButtonOkClicked()
{
    const auto selectedRows = ui.tableWidgetMonsters->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        m_selectedMonsterId = selectedRows[0].data().toString().toStdString();
        accept();
    }
}

void MonsterPickerForm::onLineEditSearchTextChanged(const QString &text)
{
   refreshMonsterTable(text.toStdString());
}

void MonsterPickerForm::onComboBoxStoreIndexChanged()
{
    refreshMonsterTable(ui.lineEditSearch->text().toStdString());
}
