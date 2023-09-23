#pragma once

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "mainForm_GLComponent.hpp"
#include "monsterStore.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTO.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "types.hpp"

struct MainForm_MonsterZoneTabComponent_Objects {
    MainForm_GLComponent *glComponent = nullptr;
    QTableWidget *tableWidgetMonsterZone = nullptr;
    QPushButton *pushButtonAddMonsterZone = nullptr;
    QPushButton *pushButtonEditMonsterZone = nullptr;
    QPushButton *pushButtonDeleteMonsterZone = nullptr;
};

class MainForm_MonsterZoneTabComponent : public QWidget {
    Q_OBJECT

 public:
    MainForm_MonsterZoneTabComponent();
    void initializeUIObjects(const MainForm_MonsterZoneTabComponent_Objects &objects);
    void connectUIActions();
    void refreshMonsterZones();
    std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    std::optional<const mapeditor::controllers::MonsterZoneDTO> getSelectedMonsterZoneInMonsterZoneList() const;
    void setMonsterStores(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    void setResourcesPath(const std::string &resourcesPath);
 signals:
    void monsterZoneAdded(mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void monsterZoneUpdated(const std::string &name, mapeditor::controllers::MonsterZoneDTO monsterZoneDTO);
    void monsterZoneDeleted(const std::string &name);

 private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores = nullptr;
    std::string m_resourcesPath;
    QTableWidgetKeyPressWatcher tableWidgetMonsterZoneKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    QTableWidget *m_tableWidgetMonsterZone;
    QPushButton *m_pushButtonAddMonsterZone;
    QPushButton *m_pushButtonEditMonsterZone;
    QPushButton *m_pushButtonDeleteMonsterZone;
    void onPushButtonAddMonsterZoneClick();
    void onPushButtonEditMonsterZoneClick();
    void onPushButtonDeleteMonsterZoneClick();
    void onTableWidgetMonsterZoneKeyPressEvent(int key, int, int);
};
