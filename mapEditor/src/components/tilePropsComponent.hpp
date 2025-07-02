#pragma once

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include <set>
#include <vector>
#include <boost/optional.hpp>
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
#include "mapTileDTO.hpp"
#include "mapTileTriggerDTO.hpp"
#include "monsterZoneDTO.hpp"
#include "qTableWidgetKeyPressWatcher.h"
#include "tilePropsComponentController.hpp"
#include "ui_tilePropsComponent.h"

class TilePropsComponent : public QWidget {
Q_OBJECT

 public:
    TilePropsComponent(QWidget *parent,
            MainForm_GLComponent *glComponent,
            mapeditor::controllers::GLComponentController *glComponentController);
    void connectUIActions();
    void reset();
    void refreshMonsterZones(const std::vector<mapeditor::controllers::MonsterZoneDTO> &zones);
    void setOnlyOneMonsterZoneForMap(bool value);
    void disableFieldsChangeEvent();
    void enableFieldsChangeEvent();

 private:
    Ui::TilePropsComponent ui;
    mapeditor::controllers::TilePropsComponentController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetMapTileTriggersKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    bool m_disableFieldsChangedEvent;
    void refreshEventList(const std::set<mapeditor::controllers::MapTileTriggerDTO> &triggers);
    void refreshMonsterZoneComboBoxEnableStatus();
    void onTileSelected(std::vector<mapeditor::controllers::MapTileDTO> tiles);
    void onTileUnselected();
    void onTilePropsChanged();
    void onTileTriggerChanged();
    void onLineEditTexNameEditingFinished();
    void onSpinBoxTexIndexEditingFinished();
    void onLineEditObjTexNameEditingFinished();
    void onSpinBoxObjTexIndexEditingFinished();
    void onCheckBoxObjectAbovePlayerChanged(int state);
    void onCheckBoxTileCanSteppedOnChanged(int state);
    void onCheckBoxIsWallToClimbChanged(int state);
    void onComboBoxMonsterZoneCurrentIndexChanged(int index);
    boost::optional<mapeditor::controllers::MapTileTriggerDTO> getSelectedTrigger();
    void onPushButtonClearMonsterZoneClick();
    void onPushButtonAddTileEventClick();
    void onPushButtonEditTileEventClick();
    void onPushButtonDeleteTileEventClick();
    void onTableWidgetMapTileTriggersKeyPressEvent(int key, int, int);
};
