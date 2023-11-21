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
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
#include "mapTileTriggerDTO.hpp"
#include "point.hpp"
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

 private:
    Ui::TilePropsComponent ui;
    mapeditor::controllers::TilePropsComponentController m_controller;
    QTableWidgetKeyPressWatcher tableWidgetMapTileTriggersKeyWatcher;
    MainForm_GLComponent *m_glComponent;
    bool m_disableFieldsChangedEvent;
    void refreshEventList(std::set<mapeditor::controllers::MapTileTriggerDTO> triggers);
    void onTileSelected(std::vector<mapeditor::controllers::MapTileDTO> tiles, thewarrior::models::Point<> coord);
    void onTileUnselected();
    void onLineEditTexNameTextChanged(const QString &text);
    void onSpinBoxTexIndexValueChanged(int value);
    void onLineEditObjTexNameTextChanged(const QString &text);
    void onSpinBoxObjTexIndexValueChanged(int value);
    void onCheckBoxObjectAbovePlayerChanged(int state);
    void onCheckBoxTileCanSteppedOnChanged(int state);
    void onCheckBoxIsWallToClimbChanged(int state);
    boost::optional<mapeditor::controllers::MapTileTriggerDTO> getSelectedTrigger();
    void onPushButtonAddTileEventClick();
    void onPushButtonEditTileEventClick();
    void onPushButtonDeleteTileEventClick();
    void onTableWidgetMapTileTriggersKeyPressEvent(int key, int, int);
};
