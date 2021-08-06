#pragma once

#include "gameMap.hpp"
#include "mapOpenGLWidget.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include <memory>
#include <QtWidgets/QWidget>
#include <string>
#include <vector>

class MainForm_GLComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_GLComponent();
    void initializeUIObjects(MapOpenGLWidget *glWidget);
    void connectUIActions();
    void setCurrentMap(std::shared_ptr<GameMap> map);
    void setSelectionMode(SelectionMode mode);
    MapTile *getCurrentMapTile(); 
    void setLastSelectedTexture(const std::string &name, int index);
    void setLastSelectedObject(const std::string &name, int index);
    void clearLastSelectedTexture();
    void clearLastSelectedObject();
signals:
    void tileSelected(MapTile *tile, Point coord);
private:
    MapOpenGLWidget *glWidget;
	std::shared_ptr<GameMap> map;
    MapTile *currentMapTile;
    std::string lastSelectedTextureName;
	std::string lastSelectedObjectName;
	int lastSelectedTextureIndex;
	int lastSelectedObjectIndex;
    void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> selectedTileIndexes);
};