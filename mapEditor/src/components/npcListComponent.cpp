#include <qwidget.h>
#include <qcheckbox.h>
#include <qnamespace.h>
#include <fmt/format.h>
#include <QStyle>
#include <memory>
#include <string>
#include "npcListComponent.hpp"
#include "editNPCForm.hpp"
#include "point.hpp"
#include "uiUtils.hpp"

using mapeditor::controllers::GLComponentController;
using thewarrior::models::Point;

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

void NPCListComponent::restoreEditForm(const Point<> &position) {
    // Make sure you did not cancel with th ESC key
    if (position != Point<>(-1, -1)) {
        m_editForm->restoreFromPicker(position);
    }
    showEditForm();
}

void NPCListComponent::onPushButtonAddNPCClick() {
    m_editForm = std::make_unique<EditNPCForm>(this,
                                               m_resourcesPath,
                                               m_glComponent->getTextures());
    showEditForm();
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

void NPCListComponent::showEditForm() {
    m_glComponent->stopAutoUpdate();
    commoneditor::ui::UIUtils::centerToScreen(m_editForm.get());
    if (m_editForm->exec() == QDialog::Accepted) {
        if (m_editForm->isSpawnPositionPickerModeEnabled()) {
            m_glComponent->npcSpawnPositionPickerModeChanged(true);
        } else {
            //TODO: 0.6.0 Regular OK action
        }
    }
    m_glComponent->startAutoUpdate();
}

