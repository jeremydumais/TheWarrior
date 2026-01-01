#include "npcListComponent.hpp"
#include <QStyle>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qnamespace.h>
#include <fmt/format.h>
#include <string>

using mapeditor::controllers::GLComponentController;

NPCListComponent::NPCListComponent(QWidget *parent,
        MainForm_GLComponent *glComponent,
        GLComponentController *glComponentController)
    : QWidget(parent),
      ui(Ui::NPCListComponent()),
      m_controller(glComponentController),
      m_glComponent(glComponent) {
      ui.setupUi(this);
      initializeUIObjects();
      connectUIActions();
}

void NPCListComponent::initializeUIObjects() {
    ui.tableWidgetNPC->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
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
}

void NPCListComponent::setResourcesPath(const std::string &resourcesPath) {
    m_resourcesPath = resourcesPath;
}

void NPCListComponent::onPushButtonAddNPCClick() {
    m_glComponent->stopAutoUpdate();
    m_glComponent->startAutoUpdate();
}

void NPCListComponent::onPushButtonEditNPCClick() {
    m_glComponent->stopAutoUpdate();
    m_glComponent->startAutoUpdate();
}

void NPCListComponent::onPushButtonDeleteNPCClick() {
}

void NPCListComponent::onTableWidgetNPCKeyPressEvent(int key, int, int) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteNPCClick();
    }
}

