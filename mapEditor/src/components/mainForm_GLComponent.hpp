#pragma once

#include "gameMap.hpp"
#include "glComponentController.hpp"
#include "../mapOpenGLWidget.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include <QtWidgets/QWidget>
#include <memory>
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
    const std::string &getResourcesPath() const;
    SelectionMode getSelectionMode() const;
    void setResourcesPath(const std::string &path);
    void setSelectionMode(SelectionMode mode);
    MapTile *getCurrentMapTile(); 
    void setLastSelectedTexture(const std::string &name, int index);
    void setLastSelectedObject(const std::string &name, int index);
    void clearLastSelectedTexture();
    void clearLastSelectedObject();
    void stopAutoUpdate();
    void startAutoUpdate();
    void updateGL();
    const std::vector<Texture>& getTextures() const;
    boost::optional<const Texture &> getTextureByName(const std::string &name) const;
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    bool isTextureUsedInMap(const std::string &name);
    void reloadTextures();
    bool isShrinkMapImpactAssignedTiles(int offsetLeft, 
                                        int offsetTop, 
                                        int offsetRight, 
                                        int offsetBottom) const;
    void resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom);
signals:
    void tileSelected(MapTile *tile, Point coord);
private:
    MapOpenGLWidget *glWidget;
    GLComponentController controller;
    MapTile *currentMapTile;
    std::string lastSelectedTextureName;
	std::string lastSelectedObjectName;
	int lastSelectedTextureIndex;
	int lastSelectedObjectIndex;
    void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> selectedTileIndexes);
    void addMoveDenyTrigger(const std::vector<int> &selectedTileIndexes, MapTileTriggerEvent event);
};