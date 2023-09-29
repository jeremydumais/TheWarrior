#include "mainForm_MonsterZoneTabComponent.hpp"
#include <QStyle>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <qcheckbox.h>
#include <qnamespace.h>
#include "editMonsterZoneForm.hpp"
#include "errorMessage.hpp"
#include "monsterZoneDTO.hpp"
#include "types.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
using mapeditor::controllers::ContainerOfMonsterStore;
using mapeditor::controllers::MonsterZoneDTO;

MainForm_MonsterZoneTabComponent::MainForm_MonsterZoneTabComponent()
    : m_glComponent(nullptr),
    m_tableWidgetMonsterZone(nullptr),
    m_pushButtonAddMonsterZone(nullptr),
    m_pushButtonEditMonsterZone(nullptr),
    m_pushButtonDeleteMonsterZone(nullptr),
    m_checkBoxOneMonsterZoneForAllTheMap(nullptr) {
}

void MainForm_MonsterZoneTabComponent::initializeUIObjects(const MainForm_MonsterZoneTabComponent_Objects &objects) {
    this->m_glComponent = objects.glComponent;
    this->m_tableWidgetMonsterZone = objects.tableWidgetMonsterZone;
    this->m_pushButtonAddMonsterZone = objects.pushButtonAddMonsterZone;
    this->m_pushButtonEditMonsterZone = objects.pushButtonEditMonsterZone;
    this->m_pushButtonDeleteMonsterZone = objects.pushButtonDeleteMonsterZone;
    this->m_checkBoxOneMonsterZoneForAllTheMap = objects.checkBoxOneMonsterZoneForAllTheMap;
    this->m_tableWidgetMonsterZone->setHorizontalHeaderItem(0, new QTableWidgetItem("Color"));
    this->m_tableWidgetMonsterZone->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    this->m_tableWidgetMonsterZone->setColumnWidth(0, 80);
    this->m_tableWidgetMonsterZone->setColumnWidth(1, 300);
}

void MainForm_MonsterZoneTabComponent::connectUIActions() {
    connect(m_pushButtonAddMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonAddMonsterZoneClick);
    connect(m_pushButtonEditMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonEditMonsterZoneClick);
    connect(m_pushButtonDeleteMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonDeleteMonsterZoneClick);
    connect(m_tableWidgetMonsterZone, &QTableWidget::itemDoubleClicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonEditMonsterZoneClick);
    tableWidgetMonsterZoneKeyWatcher.installOn(m_tableWidgetMonsterZone);
    connect(&tableWidgetMonsterZoneKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &MainForm_MonsterZoneTabComponent::onTableWidgetMonsterZoneKeyPressEvent);
    connect(m_checkBoxOneMonsterZoneForAllTheMap, &QCheckBox::stateChanged, this, &MainForm_MonsterZoneTabComponent::onCheckBoxOneMonsterZoneForAllTheMapChanged);
}

void MainForm_MonsterZoneTabComponent::refreshMonsterZones() {
    m_tableWidgetMonsterZone->model()->removeRows(0, m_tableWidgetMonsterZone->rowCount());
    int index {0};
     for (const auto &monsterZone : m_glComponent->getMonsterZones()) {
        m_tableWidgetMonsterZone->insertRow(index);
        m_tableWidgetMonsterZone->setItem(index, 0, new QTableWidgetItem(monsterZone.m_colorName.c_str()));
        m_tableWidgetMonsterZone->setItem(index, 1, new QTableWidgetItem(monsterZone.m_name.c_str()));
         index++;
     }
}

std::vector<MonsterZoneDTO> MainForm_MonsterZoneTabComponent::getMonsterZones() const {
    return m_glComponent->getMonsterZones();
}
std::optional<const MonsterZoneDTO> MainForm_MonsterZoneTabComponent::getSelectedMonsterZoneInMonsterZoneList() const {
    if (m_tableWidgetMonsterZone->selectionModel()->hasSelection()) {
        // Find the selected monster zone
        const auto selectedRow = m_tableWidgetMonsterZone->selectionModel()->selectedRows()[0];
        auto selectedItemName { selectedRow.sibling(selectedRow.row(), 1).data().toString().toStdString() };
        return m_glComponent->getMonsterZoneByName(selectedItemName);
    } else {
        return std::nullopt;
    }
}

void MainForm_MonsterZoneTabComponent::setMonsterStores(const std::shared_ptr<ContainerOfMonsterStore> monsterStores) {
    m_monsterStores = monsterStores;
}

void MainForm_MonsterZoneTabComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void MainForm_MonsterZoneTabComponent::confirmValidityOfOneMonsterZoneCheckBox() {
    const auto &zones = m_glComponent->getMonsterZones();
    if (zones.size() != 1 &&
        m_checkBoxOneMonsterZoneForAllTheMap->checkState() == Qt::CheckState::Checked) {
        m_checkBoxOneMonsterZoneForAllTheMap->setCheckState(Qt::CheckState::Unchecked);
    }
}

std::string MainForm_MonsterZoneTabComponent::getMonsterZoneColor(const std::string &zoneName) const {
    const auto &zone = m_glComponent->getMonsterZoneByName(zoneName);
    if (zone.has_value()) {
        return zone->m_colorValue;
    }
    return "";
}

bool MainForm_MonsterZoneTabComponent::isMonsterZonesEmpty() const {
    return m_glComponent->getMonsterZones().empty();
}

void MainForm_MonsterZoneTabComponent::onPushButtonAddMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    const auto alreadyUsedMonsterZoneNames = m_glComponent->getAlreadyUsedMonsterZoneNames();
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

void MainForm_MonsterZoneTabComponent::onPushButtonEditMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        auto alreadyUsedMonsterZoneNames = m_glComponent->getAlreadyUsedMonsterZoneNames();
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
            // TODO: Check if the monster zone is used in the map
        }
    }
    m_glComponent->startAutoUpdate();
}

void MainForm_MonsterZoneTabComponent::onPushButtonDeleteMonsterZoneClick() {
    auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the monster zone {0}?", selectedMonsterZone->m_name).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
                emit monsterZoneDeleted(selectedMonsterZone->m_name);
            // TODO: Check if the monster zone is used in the map
            //msgBox.setText(fmt::format("The monster zone {0} is used by some map tiles.\nAre you sure you want to proceed?", selectedMonsterZone->get().getName()).c_str());
            //bool isUsed = m_glComponent->isMonsterZoneUsedInMap(selectedMonsterZone->get().getName());
            //if (!isUsed || (isUsed && msgBox.exec() == QMessageBox::Yes)) {
                //emit textureDeleted(selectedMonsterZone->get().getName());
            //}
        }
    }
}

void MainForm_MonsterZoneTabComponent::onTableWidgetMonsterZoneKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteMonsterZoneClick();
    }
}

void MainForm_MonsterZoneTabComponent::onCheckBoxOneMonsterZoneForAllTheMapChanged(int state) {
    if (state == Qt::CheckState::Checked) {
        const auto &zones = m_glComponent->getMonsterZones();
        if (zones.size() != 1) {
            ErrorMessage::show("To enable this feature you must have exactly one monster zone configured");
            m_checkBoxOneMonsterZoneForAllTheMap->setCheckState(Qt::CheckState::Unchecked);
            return;
        } else {
            emit useOnlyOneMonsterZoneChanged(true);
        }
    } else {
        emit useOnlyOneMonsterZoneChanged(false);
    }
}
