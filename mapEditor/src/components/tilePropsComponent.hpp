#pragma once

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include <vector>
#include <boost/optional.hpp>
#include "glComponentController.hpp"
#include "mainForm_GLComponent.hpp"
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
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
    // TODO: 0.3.3 Implement the controllers
    bool m_disableFieldsChangedEvent;
    void refreshEventList(thewarrior::models::MapTile *tile);
    void onTileSelected(std::vector<mapeditor::controllers::MapTileDTO> tiles, thewarrior::models::Point<> coord);
    void onTileUnselected();
    void onLineEditTexNameTextChanged(const QString &text);
    void onSpinBoxTexIndexValueChanged(int value);
    void onLineEditObjTexNameTextChanged(const QString &text);
    void onSpinBoxObjTexIndexValueChanged(int value);
    void onCheckBoxObjectAbovePlayerChanged(int state);
    void onCheckBoxTileCanSteppedOnChanged(int state);
    void onCheckBoxIsWallToClimbChanged(int state);
    boost::optional<thewarrior::models::MapTileTrigger &> getSelectedTrigger();
    void onPushButtonAddTileEventClick();
    void onPushButtonEditTileEventClick();
    void onPushButtonDeleteTileEventClick();
    void onTableWidgetMapTileTriggersKeyPressEvent(int key, int, int);
};
