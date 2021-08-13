#pragma once

#include "mainForm_GLComponent.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include <boost/optional.hpp>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

struct MainForm_TileTabComponent_Objects
{
    MainForm_GLComponent *glComponent = nullptr;
    QLabel *labelTileCoordXY = nullptr;
	QLineEdit *lineEditTexName = nullptr;
    QSpinBox *spinBoxTexIndex = nullptr;
	QLineEdit *lineEditObjTexName = nullptr;
	QSpinBox *spinBoxObjTexIndex = nullptr;
	QCheckBox *checkBoxTileCanSteppedOn = nullptr;
	QCheckBox *checkBoxObjectAbovePlayer = nullptr;
	QListWidget *listWidgetMapTileTriggers = nullptr;
	QPushButton *pushButtonAddTileEvent = nullptr;
	QPushButton *pushButtonEditTileEvent = nullptr;
	QPushButton *pushButtonDeleteTileEvent = nullptr;
};

class MainForm_TileTabComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_TileTabComponent();
    void initializeUIObjects(const MainForm_TileTabComponent_Objects &objects);
    void connectUIActions();
private:
    MainForm_GLComponent *glComponent;
    QLabel *labelTileCoordXY;
	QLineEdit *lineEditTexName;
    QSpinBox *spinBoxTexIndex;
	QLineEdit *lineEditObjTexName;
	QSpinBox *spinBoxObjTexIndex;
	QCheckBox *checkBoxTileCanSteppedOn;
	QCheckBox *checkBoxObjectAbovePlayer;
	QListWidget *listWidgetMapTileTriggers;
	QPushButton *pushButtonAddTileEvent;
	QPushButton *pushButtonEditTileEvent;
	QPushButton *pushButtonDeleteTileEvent;
	void refreshEventList(MapTile *tile);
    void onTileSelected(MapTile *tile, Point coord);
    void onLineEditTexNameTextChanged(const QString &text);
	void onSpinBoxTexIndexValueChanged(int value);
	void onLineEditObjTexNameTextChanged(const QString &text);
	void onSpinBoxObjTexIndexValueChanged(int value);
	void onCheckBoxObjectAbovePlayerChanged(int state);
	void onCheckBoxTileCanSteppedOnChanged(int state);
	boost::optional<MapTileTrigger &> getSelectedTrigger();
	void onPushButtonAddTileEventClick();
	void onPushButtonEditTileEventClick();
};