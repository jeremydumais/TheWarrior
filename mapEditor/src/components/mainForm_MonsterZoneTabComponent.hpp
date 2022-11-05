#pragma once

#include "mainForm_GLComponent.hpp"
#include "monsterStore.hpp"
#include "types.hpp"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <vector>

struct MainForm_MonsterZoneTabComponent_Objects
{
    MainForm_GLComponent *glComponent = nullptr;
    QTableWidget *tableWidgetMonsterZone = nullptr;
    QPushButton *pushButtonAddMonsterZone = nullptr;
    QPushButton *pushButtonEditMonsterZone = nullptr;
    QPushButton *pushButtonDeleteMonsterZone = nullptr;
};

class MainForm_MonsterZoneTabComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_MonsterZoneTabComponent();
    void initializeUIObjects(const MainForm_MonsterZoneTabComponent_Objects &objects);
    void connectUIActions();
    void refreshMonsterZones();
    //std::optional<std::reference_wrapper<const thewarrior::models::MonsterZone>> getSelectedMonsterZoneInMonsterZoneList() const;
    void setMonsterStores(const std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> monsterStores);
    void onPushButtonAddMonsterZoneClick();
signals:
    //void textureAdded(thewarrior::models::MonsterZoneInfo textureInfo);
    //void textureUpdated(const std::string &name, thewarrior::models::MonsterZoneInfo textureInfo);
    //void textureDeleted(const std::string &name);
private:
    std::shared_ptr<mapeditor::controllers::ContainerOfMonsterStore> m_monsterStores = nullptr;
    MainForm_GLComponent *m_glComponent;
    QTableWidget *m_tableWidgetMonsterZone;
    QPushButton *m_pushButtonAddMonsterZone;
    QPushButton *m_pushButtonEditMonsterZone;
    QPushButton *m_pushButtonDeleteMonsterZone;
    void onPushButtonEditMonsterZoneClick();
	void onPushButtonDeleteMonsterZoneClick();
};
