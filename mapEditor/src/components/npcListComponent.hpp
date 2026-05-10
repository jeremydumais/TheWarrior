#pragma once

#include <qcheckbox.h>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "editNPCForm.hpp"
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
#include "npcDTO.hpp"
#include "npcListComponentController.hpp"
#include "point.hpp"
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
    bool isNPCListEmpty() const;
    std::vector<mapeditor::controllers::NPCDTO> getNPCs() const;
    std::optional<const mapeditor::controllers::NPCDTO> getSelectedNPC() const;
    void setResourcesPath(const std::string &resourcesPath);
    void restoreEditForm(const thewarrior::models::Point<> &position);
 signals:
    void npcAdded(mapeditor::controllers::NPCDTO npcDTO);
    void npcUpdated(const std::string &id, mapeditor::controllers::NPCDTO npcDTO);
    void npcDeleted(const std::string &id);

 private:
    Ui::NPCListComponent ui;
    mapeditor::controllers::NPCListComponentController m_controller;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetNPCKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    std::unique_ptr<EditNPCForm> m_editForm = nullptr;
    void onPushButtonAddNPCClick();
    void onPushButtonEditNPCClick();
    void onPushButtonDeleteNPCClick();
    void onTableWidgetNPCKeyPressEvent(int key, int, int);
    void showEditForm();
};
