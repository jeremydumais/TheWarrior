#include "mainForm_MonsterZoneTabComponent.hpp"
#include <QStyle>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include "editMonsterZoneForm.hpp"
#include "errorMessage.hpp"
#include "monsterZoneDTO.hpp"
#include "types.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::UIUtils;
using mapeditor::controllers::ContainerOfMonsterStore;
using mapeditor::controllers::MonsterZoneDTO;

MainForm_MonsterZoneTabComponent::MainForm_MonsterZoneTabComponent()
    : m_glComponent(nullptr),
    m_tableWidgetMonsterZone(nullptr),
    m_pushButtonAddMonsterZone(nullptr),
    m_pushButtonEditMonsterZone(nullptr),
    m_pushButtonDeleteMonsterZone(nullptr) {
}

void MainForm_MonsterZoneTabComponent::initializeUIObjects(const MainForm_MonsterZoneTabComponent_Objects &objects) {
    this->m_glComponent = objects.glComponent;
    this->m_tableWidgetMonsterZone = objects.tableWidgetMonsterZone;
    this->m_pushButtonAddMonsterZone = objects.pushButtonAddMonsterZone;
    this->m_pushButtonEditMonsterZone = objects.pushButtonEditMonsterZone;
    this->m_pushButtonDeleteMonsterZone = objects.pushButtonDeleteMonsterZone;
    this->m_tableWidgetMonsterZone->setHorizontalHeaderItem(0, new QTableWidgetItem("Color"));
    this->m_tableWidgetMonsterZone->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    this->m_tableWidgetMonsterZone->setColumnWidth(0, 80);
    this->m_tableWidgetMonsterZone->setColumnWidth(1, 300);
}

void MainForm_MonsterZoneTabComponent::connectUIActions() {
    connect(m_pushButtonAddMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonAddMonsterZoneClick);
    connect(m_pushButtonEditMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonEditMonsterZoneClick);
    connect(m_pushButtonDeleteMonsterZone, &QPushButton::clicked, this, &MainForm_MonsterZoneTabComponent::onPushButtonDeleteMonsterZoneClick);
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
             // emit textureUpdated(selectedMonsterZone->get().getName(), formEditMonsterZone.getMonsterZoneInfo());
         }
    }
    m_glComponent->startAutoUpdate();
}

void MainForm_MonsterZoneTabComponent::onPushButtonDeleteMonsterZoneClick() {
    /*auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the texture {0}?", selectedMonsterZone->get().getName()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            //Check if the texture is used in the map
            msgBox.setText(fmt::format("The texture {0} is used by some map tiles.\nAre you sure you want to proceed?", selectedMonsterZone->get().getName()).c_str());
            bool isUsed = m_glComponent->isMonsterZoneUsedInMap(selectedMonsterZone->get().getName());
            if (!isUsed || (isUsed && msgBox.exec() == QMessageBox::Yes)) {
                emit textureDeleted(selectedMonsterZone->get().getName());
            }
        }
    }*/
}
