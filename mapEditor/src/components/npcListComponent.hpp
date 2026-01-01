#pragma once

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <qcheckbox.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
//#include "npcDTO.hpp"
//#include "npcListComponentController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "ui_npcListComponent.h"
#include "types.hpp"

class NPCListComponent : public QWidget {
    Q_OBJECT

 public:
    NPCListComponent(QWidget *parent,
            MainForm_GLComponent *glComponent,
            mapeditor::controllers::GLComponentController *glComponentController);
    void initializeUIObjects();
    void connectUIActions();
    void refreshNPCs();
    //void disableFieldsChangeEvent();
    //void enableFieldsChangeEvent();
    //std::vector<mapeditor::controllers::NPCDTO> getMonsterZones() const;
    //std::optional<const mapeditor::controllers::NPCDTO> getSelectedMonsterZoneInMonsterZoneList() const;
    //void setMonsterStores(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    void setResourcesPath(const std::string &resourcesPath);
    //bool isNPCsEmpty() const;
 signals:
    //void npcAdded(mapeditor::controllers::NPCDTO npcDTO);
    //void npcUpdated(const std::string &name, mapeditor::controllers::NPCDTO npcDTO);
    //void npcDeleted(const std::string &name);

 private:
    Ui::NPCListComponent ui;
    //mapeditor::controllers::NPCListComponentController m_controller;
    //std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores = nullptr;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetNPCKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    //bool m_disableFieldsChangedEvent;
    void onPushButtonAddNPCClick();
    void onPushButtonEditNPCClick();
    void onPushButtonDeleteNPCClick();
    void onTableWidgetNPCKeyPressEvent(int key, int, int);
};
