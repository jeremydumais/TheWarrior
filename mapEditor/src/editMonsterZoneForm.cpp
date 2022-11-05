#include "editMonsterZoneForm.hpp"
#include "editMonsterEncounterForm.hpp"
#include "errorMessage.hpp"
#include "manageMonsterStoreController.hpp"
#include "types.hpp"
#include <cstddef>
#include <exception>
#include <fmt/format.h>
#include <qapplication.h>
#include <memory>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;

EditMonsterZoneForm::EditMonsterZoneForm(QWidget *parent,
                                         const std::shared_ptr<ContainerOfMonsterStore> monsterStores)
    : QDialog(parent),
    ui(Ui::editMonsterZoneFormClass()),
    m_controller(monsterStores)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    initializeColors();
    initializeMonsterTable();
    onPushButtonAddMonsterClick();
}

void EditMonsterZoneForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, std::terminate);//&EditMonsterZoneForm::reject);
    connect(ui.comboBoxColor, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EditMonsterZoneForm::onComboBoxColorCurrentIndexChanged);
    connect(ui.pushButtonAddMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonAddMonsterClick);
    connect(ui.pushButtonEditMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonEditMonsterClick);
    connect(ui.pushButtonDeleteMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonDeleteMonsterClick);
}

void EditMonsterZoneForm::initializeColors()
{
    ui.comboBoxColor->model()->removeRows(0, ui.comboBoxColor->count());
    for (const auto &color : colors) {
        ui.comboBoxColor->addItem(color.displayName.c_str());
    }
}

void EditMonsterZoneForm::initializeMonsterTable()
{
    ui.tableWidgetMonsters->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetMonsters->setHorizontalHeaderItem(2, new QTableWidgetItem("Ratio"));
    ui.tableWidgetMonsters->setColumnWidth(0, 80);
    ui.tableWidgetMonsters->setColumnWidth(1, 325);
    ui.tableWidgetMonsters->setColumnWidth(2, 50);
}

void EditMonsterZoneForm::onPushButtonOK()
{
}

void EditMonsterZoneForm::onComboBoxColorCurrentIndexChanged()
{
    //Find the selected color
    auto selectedIndex = ui.comboBoxColor->currentIndex();
    if (selectedIndex >= 0) {
        size_t colorIndex = static_cast<size_t>(selectedIndex);
        if (colorIndex < colors.size()) {
            ui.labelColor->setStyleSheet(fmt::format("background-color: {0}",
                                                     colors[colorIndex].value).c_str());
            return;
        }
    }
    ui.labelColor->setStyleSheet("");
}

void EditMonsterZoneForm::onPushButtonAddMonsterClick()
{
    EditMonsterEncounterForm monsterEncounterForm(this, m_controller.getMonsterStores());
    monsterEncounterForm.exec();
}

void EditMonsterZoneForm::onPushButtonEditMonsterClick()
{

}

void EditMonsterZoneForm::onPushButtonDeleteMonsterClick()
{

}
