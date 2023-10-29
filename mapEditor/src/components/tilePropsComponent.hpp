#pragma once

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include <boost/optional.hpp>
#include "mainForm_GLComponent.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include "ui_tilePropsComponent.h"

class TilePropsComponent : public QWidget {
Q_OBJECT
 public:
    TilePropsComponent(QWidget *parent,
            MainForm_GLComponent *glComponent);
    void connectUIActions();
    void reset();

 private:
    Ui::TilePropsComponent ui;
    MainForm_GLComponent *m_glComponent;
    void refreshEventList(thewarrior::models::MapTile *tile);
    void onTileSelected(thewarrior::models::MapTile *tile, thewarrior::models::Point<> coord);
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
};
