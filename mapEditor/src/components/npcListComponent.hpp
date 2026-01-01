#pragma once

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <qcheckbox.h>
#include <string>
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
#include "npcListComponentController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_npcListComponent.h"

class NPCListComponent : public QWidget {
    Q_OBJECT

 public:
    NPCListComponent(QWidget *parent,
            MainForm_GLComponent *glComponent,
            mapeditor::controllers::GLComponentController *glComponentController);
    void initializeUIObjects();
    void connectUIActions();
    void refreshNPCs();
    void setResourcesPath(const std::string &resourcesPath);
 signals:
    //void npcAdded(mapeditor::controllers::NPCDTO npcDTO);
    //void npcUpdated(const std::string &name, mapeditor::controllers::NPCDTO npcDTO);
    //void npcDeleted(const std::string &name);

 private:
    Ui::NPCListComponent ui;
    mapeditor::controllers::NPCListComponentController m_controller;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetNPCKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    void onPushButtonAddNPCClick();
    void onPushButtonEditNPCClick();
    void onPushButtonDeleteNPCClick();
    void onTableWidgetNPCKeyPressEvent(int key, int, int);
};
