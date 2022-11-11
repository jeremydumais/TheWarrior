#include "editMonsterZoneForm.hpp"
#include "editMonsterEncounterForm.hpp"
#include "editMonsterZoneFormController.hpp"
#include "errorMessage.hpp"
#include "manageMonsterStoreController.hpp"
#include "monsterEncounterDTO.hpp"
#include "types.hpp"
#include <cstddef>
#include <exception>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <qdialog.h>
#include <qmessagebox.h>

using namespace commoneditor::ui;
using namespace mapeditor::controllers;

EditMonsterZoneForm::EditMonsterZoneForm(QWidget *parent,
                                         const std::shared_ptr<ContainerOfMonsterStore> monsterStores,
                                         const std::string resourcesPath)
    : QDialog(parent),
    ui(Ui::editMonsterZoneFormClass()),
    m_controller(monsterStores, resourcesPath)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    initializeColors();
    initializeMonsterTable();
    //TODO To remove only for tests
    //m_controller.addMonsterEncounter(MonsterEncounterDTO { "DRA001", "", "Normal", std::nullopt });
    //m_controller.addMonsterEncounter(MonsterEncounterDTO { "slm001", "", "Rare", std::nullopt });
    refreshMonsterEncounterList();
}

void EditMonsterZoneForm::connectUIActions()
{
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonOK);
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, std::terminate);//&EditMonsterZoneForm::reject);
    connect(ui.comboBoxColor, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EditMonsterZoneForm::onComboBoxColorCurrentIndexChanged);
    connect(ui.pushButtonAddMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonAddMonsterClick);
    connect(ui.pushButtonEditMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonEditMonsterClick);
    connect(ui.pushButtonDeleteMonster, &QPushButton::clicked, this, &EditMonsterZoneForm::onPushButtonDeleteMonsterClick);
    connect(ui.tableWidgetMonsters, &QTableWidget::itemDoubleClicked, this, &EditMonsterZoneForm::onPushButtonEditMonsterClick);
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

void EditMonsterZoneForm::refreshMonsterEncounterList()
{
    ui.tableWidgetMonsters->model()->removeRows(0, ui.tableWidgetMonsters->rowCount());
    int index = 0;
    for(const auto &dto : m_controller.getMonsterEncounters()) {
        ui.tableWidgetMonsters->insertRow(index);
        auto idMonster = new QTableWidgetItem(dto.monsterId.c_str());
        if (dto.monsterIcon.has_value()) {
            idMonster->setIcon(dto.monsterIcon.value());
        }
        ui.tableWidgetMonsters->setItem(index, 0, idMonster);
        ui.tableWidgetMonsters->setItem(index, 1, new QTableWidgetItem(dto.monsterName.c_str()));
        ui.tableWidgetMonsters->setItem(index, 2, new QTableWidgetItem(dto.encounterRatio.c_str()));
        index++;
    }
}

void EditMonsterZoneForm::onPushButtonOK()
{
    //TODO Required fields and at least one monster
    if (ui.lineEditName->text().trimmed().isEmpty()) {
        ErrorMessage::show("The name is required.");
        return;
    }
    if (ui.spinBoxRatioChance->value() > ui.spinBoxRatioTotal->value()) {
        ErrorMessage::show("The ratio to encounter a monster must be smaller than to total chance.");
        return;
    }
    if (ui.spinBoxRatioTotal->value() == 0) {
        ErrorMessage::show("The total chance must be at least 1.");
        return;
    }
    if (m_controller.getMonsterEncounterIds().size() == 0) {
        ErrorMessage::show("You must add at least one monster.");
        return;
    }
    accept();
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
    EditMonsterEncounterForm monsterEncounterForm(this,
                                                  m_controller.getMonsterStores(),
                                                  m_controller.getResourcesPath(),
                                                  m_controller.getMonsterEncounterIds(),
                                                  std::nullopt);
    if (monsterEncounterForm.exec() == QDialog::Accepted) {
        if (!m_controller.addMonsterEncounter(monsterEncounterForm.getResult())) {
            ErrorMessage::show(m_controller.getLastError());
        }
        refreshMonsterEncounterList();
    }
}

void EditMonsterZoneForm::onPushButtonEditMonsterClick()
{
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
        const auto selectedRow = ui.tableWidgetMonsters->selectionModel()->selectedRows()[0];
        const auto oldMonsterId = selectedRow.data().toString().toStdString();
        const auto ratio = selectedRow.sibling(selectedRow.row(), 2).data().toString().toStdString();
        MonsterEncounterDTO itemToEdit {
            .monsterId = itemId.value(),
            .monsterName = "",
            .encounterRatio = ratio,
            .monsterIcon = std::nullopt
        };
        EditMonsterEncounterForm monsterEncounterForm(this,
                                                      m_controller.getMonsterStores(),
                                                      m_controller.getResourcesPath(),
                                                      m_controller.getMonsterEncounterIds(),
                                                      itemToEdit);
        if (monsterEncounterForm.exec() == QDialog::Accepted) {
            if (!m_controller.updateMonsterEncounter(oldMonsterId, monsterEncounterForm.getResult())) {
                ErrorMessage::show(m_controller.getLastError());
            }
            refreshMonsterEncounterList();
        }
    }
}

void EditMonsterZoneForm::onPushButtonDeleteMonsterClick()
{
    if (auto itemId = getSelectedItemId(); itemId.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the monster encounter {}?", itemId.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (!m_controller.removeMonsterEncounter(itemId.value())) {
                ErrorMessage::show(m_controller.getLastError());
            }
            refreshMonsterEncounterList();
        }
    }
}

std::optional<std::string> EditMonsterZoneForm::getSelectedItemId() const
{
    auto selectedRows = ui.tableWidgetMonsters->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}

