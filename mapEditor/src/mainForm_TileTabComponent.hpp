#pragma once

#include "mainForm_GLComponent.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

struct MainForm_TileTabComponent_Objects
{
    MainForm_GLComponent *glComponent;
    QLabel *labelTileCoordXY;
	QLineEdit *lineEditTexName;
    QSpinBox *spinBoxTexIndex;
	QLineEdit *lineEditObjTexName;
	QSpinBox *spinBoxObjTexIndex;
	QCheckBox *checkBoxTileCanSteppedOn;
	QCheckBox *checkBoxObjectAbovePlayer;
};

class MainForm_TileTabComponent : public QObject
{
public:
    MainForm_TileTabComponent();
    void initializeUIObjects(const MainForm_TileTabComponent_Objects &objects);
    void connectUIActions();
    void onTileSelected(MapTile *tile, Point coord);
private:
    MainForm_GLComponent *glComponent;
    QLabel *labelTileCoordXY;
	QLineEdit *lineEditTexName;
    QSpinBox *spinBoxTexIndex;
	QLineEdit *lineEditObjTexName;
	QSpinBox *spinBoxObjTexIndex;
	QCheckBox *checkBoxTileCanSteppedOn;
	QCheckBox *checkBoxObjectAbovePlayer;
};