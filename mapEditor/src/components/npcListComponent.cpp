#include "npcListComponent.hpp"
#include <QStyle>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qnamespace.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
//#include "editNPCForm.hpp"
#include "errorMessage.hpp"
//#include "npcDTO.hpp"
#include "uiUtils.hpp"

using commoneditor::ui::ErrorMessage;
using commoneditor::ui::UIUtils;
//using mapeditor::controllers::ContainerOfMonsterStore;
using mapeditor::controllers::GLComponentController;
//using mapeditor::controllers::NPCDTO;

NPCListComponent::NPCListComponent(QWidget *parent,
        MainForm_GLComponent *glComponent,
        GLComponentController *glComponentController)
    : QWidget(parent),
      ui(Ui::NPCListComponent()),
      //m_controller(glComponentController),
      m_glComponent(glComponent) {
      //m_disableFieldsChangedEvent(false) {
      ui.setupUi(this);
      initializeUIObjects();
      connectUIActions();
}

void NPCListComponent::initializeUIObjects() {
    ui.tableWidgetNPC->setHorizontalHeaderItem(0, new QTableWidgetItem("Color"));
    ui.tableWidgetNPC->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    ui.tableWidgetNPC->setColumnWidth(0, 80);
}

void NPCListComponent::connectUIActions() {
    connect(ui.pushButtonAddNPC, &QPushButton::clicked, this, &NPCListComponent::onPushButtonAddNPCClick);
    connect(ui.pushButtonEditNPC, &QPushButton::clicked, this, &NPCListComponent::onPushButtonEditNPCClick);
    connect(ui.pushButtonDeleteNPC, &QPushButton::clicked, this, &NPCListComponent::onPushButtonDeleteNPCClick);
    connect(ui.tableWidgetNPC, &QTableWidget::itemDoubleClicked, this, &NPCListComponent::onPushButtonEditNPCClick);
    tableWidgetNPCKeyWatcher.installOn(ui.tableWidgetNPC);
    connect(&tableWidgetNPCKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &NPCListComponent::onTableWidgetNPCKeyPressEvent);
}

void NPCListComponent::refreshNPCs() {
    //ui.tableWidgetNPC->model()->removeRows(0, ui.tableWidgetMonsterZone->rowCount());
    //int index {0};
    //for (const auto &npc : m_controller.getNPCs()) {
        //ui.tableWidgetNPC->insertRow(index);
        //ui.tableWidgetNPC->setItem(index, 0, new QTableWidgetItem(npc.m_colorName.c_str()));
        //ui.tableWidgetNPC->setItem(index, 1, new QTableWidgetItem(npc.m_name.c_str()));
         //index++;
    //}
}

//void NPCListComponent::disableFieldsChangeEvent() {
    //m_disableFieldsChangedEvent = true;
//}

//void NPCListComponent::enableFieldsChangeEvent() {
    //m_disableFieldsChangedEvent = false;
//}


void NPCListComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void NPCListComponent::onPushButtonAddNPCClick() {
    m_glComponent->stopAutoUpdate();
    //const auto alreadyUsedNPCNames = m_controller.getAlreadyUsedMonsterZoneNames();
    //EditNPCForm formEditMonsterZone(this,
            //m_monsterStores,
            //m_resourcesPath,
            //std::nullopt,
            //alreadyUsedNPCNames);
    //UIUtils::centerToScreen(&formEditNPC);
    //if (formEditNPC.exec() == QDialog::Accepted) {
        //emit npcAdded(formEditNPC.getResult());
    //}
    m_glComponent->startAutoUpdate();
}

void NPCListComponent::onPushButtonEditNPCClick() {
    m_glComponent->stopAutoUpdate();
    //auto selectedNPC = getSelectedMonsterZoneInMonsterZoneList();
    //if (selectedNPC.has_value()) {
        //auto alreadyUsedNPCNames = m_controller.getAlreadyUsedMonsterZoneNames();
        //// Remove the actual selected monster zone name
        //auto iter = std::find(alreadyUsedNPCNames.begin(), alreadyUsedMonsterZoneNames.end(), selectedMonsterZone.value().m_name);
        //if (iter != alreadyUsedNPCNames.end()) {
            //alreadyUsedNPCNames.erase(iter);
        //}
        //EditNPCForm formEditMonsterZone(this,
                //m_monsterStores,
                //m_resourcesPath,
                //selectedNPC,
                //alreadyUsedNPCNames);
        //if (formEditNPC.exec() == QDialog::Accepted) {
            //emit npcUpdated(selectedNPC->m_name, formEditMonsterZone.getResult());
        //}
    //}
    m_glComponent->startAutoUpdate();
}

void NPCListComponent::onPushButtonDeleteNPCClick() {
    //auto selectedNPC = getSelectedMonsterZoneInMonsterZoneList();
    //if (selectedNPC.has_value()) {
        //QMessageBox msgBox;
        //msgBox.setText(fmt::format("Are you sure you want to delete the monster zone {0}?", selectedNPC->m_name).c_str());
        //msgBox.setWindowTitle("Confirmation");
        //msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        //msgBox.setDefaultButton(QMessageBox::Cancel);
        //if (msgBox.exec() == QMessageBox::Yes) {
            //emit npcDeleted(selectedNPC->m_name);
        //}
    //}
}

void NPCListComponent::onTableWidgetNPCKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteNPCClick();
    }
}

