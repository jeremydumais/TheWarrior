#include "monsterPickerForm.hpp"
#include "types.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <fmt/format.h>
#include <QString>
#include <memory>

using namespace mapeditor::controllers;
using namespace boost::algorithm;

MonsterPickerForm::MonsterPickerForm(QWidget *parent,
                                     const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : QDialog(parent),
    ui(Ui::monsterPickerFormClass()),
    m_controller(monsterStores)
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
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, std::terminate);//&EditMonsterEncounterForm::reject);
    connect(ui.lineEditSearch, &QLineEdit::textChanged, this, &MonsterPickerForm::onLineEditSearchTextChanged);
    connect(ui.comboBoxStore, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MonsterPickerForm::onComboBoxStoreIndexChanged);

}

void MonsterPickerForm::populateMonsterStoreComboBox()
{
    ui.comboBoxStore->model()->removeRows(0, ui.comboBoxStore->count());
    const auto stores = m_controller.getMonsterStores();
    if (stores != nullptr) {
        for(const auto &storeWithName : *stores) {
            ui.comboBoxStore->addItem(storeWithName.first.c_str());
        }
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
    const auto stores = m_controller.getMonsterStores();
    if (ui.comboBoxStore->currentIndex() != -1 && stores != nullptr && stores->find(selectedStoreName) != stores->end()) {
        const auto store = stores->find(selectedStoreName)->second;
        for (const auto &monster : store->getMonsters()) {;
            if (icontains(monster->getId(), filter) || icontains(monster->getName(), filter) ) {
                ui.tableWidgetMonsters->insertRow(index);
                ui.tableWidgetMonsters->setItem(index, 0, new QTableWidgetItem(monster->getId().c_str()));
                ui.tableWidgetMonsters->setItem(index, 1, new QTableWidgetItem(monster->getName().c_str()));
                ui.tableWidgetMonsters->setItem(index, 2, new QTableWidgetItem(fmt::format("{}", monster->getHealth()).c_str()));
                ui.tableWidgetMonsters->setItem(index, 3, new QTableWidgetItem(fmt::format("{}", monster->getAttack()).c_str()));
                ui.tableWidgetMonsters->setItem(index, 4, new QTableWidgetItem(fmt::format("{}", monster->getDefense()).c_str()));
                ui.tableWidgetMonsters->setItem(index, 5, new QTableWidgetItem(fmt::format("[{}-{}]",
                                                                                           monster->getGoldRewardRange().first,
                                                                                           monster->getGoldRewardRange().second).c_str()));
                index++;
            }
        }
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
