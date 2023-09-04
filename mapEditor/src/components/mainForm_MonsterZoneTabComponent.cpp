#include "mainForm_MonsterZoneTabComponent.hpp"
#include <QStyle>
#include <fmt/format.h>
#include <memory>
#include "editMonsterZoneForm.hpp"
#include "errorMessage.hpp"
#include "types.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::UIUtils;
using mapeditor::controllers::ContainerOfMonsterStore;

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
    // TODO(jed): 2023-09-04
    m_tableWidgetMonsterZone->model()->removeRows(0, m_tableWidgetMonsterZone->rowCount());
    int index {0};
    // for(const auto &texture : m_glComponent->getMonsterZones()) {
        m_tableWidgetMonsterZone->insertRow(index);
        m_tableWidgetMonsterZone->setItem(index, 0, new QTableWidgetItem("Color1"));
        m_tableWidgetMonsterZone->setItem(index, 1, new QTableWidgetItem("Name1"));
        // index++;
    // }
}

/*
 *optional<reference_wrapper<const MonsterZone>> MainForm_MonsterZoneTabComponent::getSelectedMonsterZoneInMonsterZoneList() const
 *{
 *    if (m_listWidgetMonsterZones->selectionModel()->hasSelection()) {
 *        Find the selected texture
 *        auto selectedItemName { m_listWidgetMonsterZones->selectionModel()->selectedRows()[0].data().toString().toStdString() };
 *        return m_glComponent->getMonsterZoneByName(selectedItemName);
 *    }
 *    else {
 *        return nullopt;
 *    }
 *}
 */

void MainForm_MonsterZoneTabComponent::setMonsterStores(const std::shared_ptr<ContainerOfMonsterStore> monsterStores) {
    m_monsterStores = monsterStores;
}

void MainForm_MonsterZoneTabComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void MainForm_MonsterZoneTabComponent::onPushButtonAddMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    // TODO(jed) Ensure we cannot create two monster zone with the same name
    /*
     *auto alreadyUsedMonsterZoneNames { m_glComponent->getAlreadyUsedMonsterZoneNames() };
     */
    EditMonsterZoneForm formEditMonsterZone(this, m_monsterStores, m_resourcesPath);
    UIUtils::centerToScreen(&formEditMonsterZone);
    if (formEditMonsterZone.exec() == QDialog::Accepted) {
        emit monsterZoneAdded(formEditMonsterZone.getResult());
    }
    m_glComponent->startAutoUpdate();
}

void MainForm_MonsterZoneTabComponent::onPushButtonEditMonsterZoneClick() {
    m_glComponent->stopAutoUpdate();
    /*auto selectedMonsterZone = getSelectedMonsterZoneInMonsterZoneList();
    if (selectedMonsterZone.has_value()) {
        auto alreadyUsedMonsterZoneNames = m_glComponent->getAlreadyUsedMonsterZoneNames();
        //Remove the actual selected texture name
        auto iter = std::find(alreadyUsedMonsterZoneNames.begin(), alreadyUsedMonsterZoneNames.end(), selectedMonsterZone->get().getName());
        if (iter != alreadyUsedMonsterZoneNames.end()) {
            alreadyUsedMonsterZoneNames.erase(iter);
        }
         formEditMonsterZone(this, m_glComponent->getResourcesPath(), &selectedMonsterZone->get(), alreadyUsedMonsterZoneNames);
        if (formEditMonsterZone.exec() == QDialog::Accepted) {
            emit textureUpdated(selectedMonsterZone->get().getName(), formEditMonsterZone.getMonsterZoneInfo());
        }
    }*/
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
