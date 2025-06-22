#include "monsterZoneListComponent.hpp"
#include <QStyle>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qnamespace.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include "editMonsterZoneForm.hpp"
#include "errorMessage.hpp"
#include "monsterZoneDTO.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::ContainerOfMonsterStore;
using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MonsterZoneDTO;

MonsterZoneListComponent::MonsterZoneListComponent(QWidget *parent,
        MainForm_GLComponent *glComponent,
        GLComponentController *glComponentController)
    : QWidget(parent),
      ui(Ui::MonsterZoneListComponent()),
      m_controller(glComponentController),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      initializeUIObjects();
      connectUIActions();
}

void MonsterZoneListComponent::initializeUIObjects() {
    ui.tableWidgetMonsterZone->setHorizontalHeaderItem(0, new QTableWidgetItem("Color"));
    ui.tableWidgetMonsterZone->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetMonsterZone->setColumnWidth(0, 80);
}

void MonsterZoneListComponent::connectUIActions() {
    connect(ui.pushButtonAddMonsterZone, &QPushButton::clicked, this, &MonsterZoneListComponent::onPushButtonAddMonsterZoneClick);
    connect(ui.pushButtonEditMonsterZone, &QPushButton::clicked, this, &MonsterZoneListComponent::onPushButtonEditMonsterZoneClick);
    connect(ui.pushButtonDeleteMonsterZone, &QPushButton::clicked, this, &MonsterZoneListComponent::onPushButtonDeleteMonsterZoneClick);
    connect(ui.tableWidgetMonsterZone, &QTableWidget::itemDoubleClicked, this, &MonsterZoneListComponent::onPushButtonEditMonsterZoneClick);
    tableWidgetMonsterZoneKeyWatcher.installOn(ui.tableWidgetMonsterZone);
    connect(&tableWidgetMonsterZoneKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &MonsterZoneListComponent::onTableWidgetMonsterZoneKeyPressEvent);
    connect(ui.checkBoxOneMonsterZoneForAllTheMap, &QCheckBox::stateChanged, this, &MonsterZoneListComponent::onCheckBoxOneMonsterZoneForAllTheMapChanged);
}

void MonsterZoneListComponent::refreshMonsterZones() {
    ui.tableWidgetMonsterZone->model()->removeRows(0, ui.tableWidgetMonsterZone->rowCount());
    int index {0};
    for (const auto &monsterZone : m_glComponent->getMonsterZones()) {
        ui.tableWidgetMonsterZone->insertRow(index);
        ui.tableWidgetMonsterZone->setItem(index, 0, new QTableWidgetItem(monsterZone.m_colorName.c_str()));
        ui.tableWidgetMonsterZone->setItem(index, 1, new QTableWidgetItem(monsterZone.m_name.c_str()));
         index++;
    }
    ui.checkBoxOneMonsterZoneForAllTheMap->setEnabled(m_glComponent->getMonsterZones().size() == 1);
    ui.checkBoxOneMonsterZoneForAllTheMap->setChecked(m_glComponent->isUseOnlyOneMonsterZone());
}

std::vector<MonsterZoneDTO> MonsterZoneListComponent::getMonsterZones() const {
    return m_controller.getMonsterZones();
}

std::optional<const MonsterZoneDTO> MonsterZoneListComponent::getSelectedMonsterZoneInMonsterZoneList() const {
    if (ui.tableWidgetMonsterZone->selectionModel()->hasSelection()) {
        // Find the selected monster zone
        const auto selectedRow = ui.tableWidgetMonsterZone->selectionModel()->selectedRows()[0];
        auto selectedItemName { selectedRow.sibling(selectedRow.row(), 1).data().toString().toStdString() };
        return m_controller.getMonsterZoneByName(selectedItemName);
    } else {
        return std::nullopt;
    }
}

void MonsterZoneListComponent::setMonsterStores(const std::shared_ptr<ContainerOfMonsterStore> monsterStores) {
    m_monsterStores = monsterStores;
}

void MonsterZoneListComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void MonsterZoneListComponent::confirmValidityOfOneMonsterZoneCheckBox() {
    const auto &zones = m_controller.getMonsterZones();
    if (zones.size() != 1 &&
        ui.checkBoxOneMonsterZoneForAllTheMap->checkState() == Qt::CheckState::Checked) {
        ui.checkBoxOneMonsterZoneForAllTheMap->setCheckState(Qt::CheckState::Unchecked);
    }
}

std::string MonsterZoneListComponent::getMonsterZoneColor(const std::string &zoneName) const {
    const auto &zone = m_controller.getMonsterZoneByName(zoneName);
    if (zone.has_value()) {
        return zone->m_colorValue;
    }
    return "";
}

bool MonsterZoneListComponent::isMonsterZonesEmpty() const {
    return m_controller.isMonsterZonesEmpty();
}

bool MonsterZoneListComponent::isOnlyOneMonsterZoneChecked() const {
    return ui.checkBoxOneMonsterZoneForAllTheMap->checkState() == Qt::CheckState::Checked;
}

void MonsterZoneListComponent::onPushButtonAddMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    const auto alreadyUsedMonsterZoneNames = m_controller.getAlreadyUsedMonsterZoneNames();
    EditMonsterZoneForm formEditMonsterZone(this,
            m_monsterStores,
            m_resourcesPath,
            std::nullopt,
            alreadyUsedMonsterZoneNames);
    UIUtils::centerToScreen(&formEditMonsterZone);
    if (formEditMonsterZone.exec() == QDialog::Accepted) {
        emit monsterZoneAdded(formEditMonsterZone.getResult());
    }
    m_glComponent->startAutoUpdate();
}

void MonsterZoneListComponent::onPushButtonEditMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        auto alreadyUsedMonsterZoneNames = m_controller.getAlreadyUsedMonsterZoneNames();
        // Remove the actual selected monster zone name
        auto iter = std::find(alreadyUsedMonsterZoneNames.begin(), alreadyUsedMonsterZoneNames.end(), selectedMonsterZone.value().m_name);
        if (iter != alreadyUsedMonsterZoneNames.end()) {
            alreadyUsedMonsterZoneNames.erase(iter);
        }
        EditMonsterZoneForm formEditMonsterZone(this,
                m_monsterStores,
                m_resourcesPath,
                selectedMonsterZone,
                alreadyUsedMonsterZoneNames);
        if (formEditMonsterZone.exec() == QDialog::Accepted) {
            emit monsterZoneUpdated(selectedMonsterZone->m_name, formEditMonsterZone.getResult());
        }
    }
    m_glComponent->startAutoUpdate();
}

void MonsterZoneListComponent::onPushButtonDeleteMonsterZoneClick() {
    auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the monster zone {0}?", selectedMonsterZone->m_name).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            emit monsterZoneDeleted(selectedMonsterZone->m_name);
        }
    }
}

void MonsterZoneListComponent::onTableWidgetMonsterZoneKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteMonsterZoneClick();
    }
}

void MonsterZoneListComponent::onCheckBoxOneMonsterZoneForAllTheMapChanged(int state) {
    if (state == Qt::CheckState::Checked) {
        const auto &zones = m_controller.getMonsterZones();
        if (zones.size() != 1) {
            ErrorMessage::show("To enable this feature you must have exactly one monster zone configured");
            ui.checkBoxOneMonsterZoneForAllTheMap->setCheckState(Qt::CheckState::Unchecked);
            return;
        } else {
            emit useOnlyOneMonsterZoneChanged(true);
        }
    } else {
        emit useOnlyOneMonsterZoneChanged(false);
    }
}
