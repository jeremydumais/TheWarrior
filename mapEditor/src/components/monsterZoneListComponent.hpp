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
#include "monsterZoneDTO.hpp"
#include "monsterZoneListComponentController.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "types.hpp"
#include "ui_monsterZoneListComponent.h"

class MonsterZoneListComponent : public QWidget {
    Q_OBJECT

 public:
    MonsterZoneListComponent(QWidget *parent,
            MainForm_GLComponent *glComponent,
            mapeditor::controllers::GLComponentController *glComponentController);
    void initializeUIObjects();
    void connectUIActions();
    void refreshMonsterZones();
    std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    std::optional<const mapeditor::controllers::MonsterZoneDTO> getSelectedMonsterZoneInMonsterZoneList() const;
    void setMonsterStores(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    void setResourcesPath(const std::string &resourcesPath);
    void confirmValidityOfOneMonsterZoneCheckBox();
    std::string getMonsterZoneColor(const std::string &zoneName) const;
    bool isMonsterZonesEmpty() const;
    bool isOnlyOneMonsterZoneChecked() const;
 signals:
    void monsterZoneAdded(mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void monsterZoneUpdated(const std::string &name, mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void monsterZoneDeleted(const std::string &name);
    void useOnlyOneMonsterZoneChanged(bool value);

 private:
    Ui::MonsterZoneListComponent ui;
    mapeditor::controllers::MonsterZoneListComponentController m_controller;
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores = nullptr;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetMonsterZoneKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    void onPushButtonAddMonsterZoneClick();
    void onPushButtonEditMonsterZoneClick();
    void onPushButtonDeleteMonsterZoneClick();
    void onTableWidgetMonsterZoneKeyPressEvent(int key, int, int);
    void onCheckBoxOneMonsterZoneForAllTheMapChanged(int state);
};
