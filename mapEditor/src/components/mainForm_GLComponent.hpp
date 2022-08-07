#pragma once

#include "gameMap.hpp"
#include "../mapOpenGLWidget.hpp"
#include "glComponentController.hpp"
#include "mapTile.hpp"
#include "point.hpp"
#include "selectionMode.hpp"
#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class MainForm_GLComponent : public QWidget
{
    Q_OBJECT
public:
    MainForm_GLComponent();
    void initializeUIObjects(MapOpenGLWidget *glWidget);
    void connectUIActions();
    const std::string &getResourcesPath() const;
    SelectionMode getSelectionMode() const;
    unsigned int getMapWidth() const;
    unsigned int getMapHeight() const;
    void setCurrentMap(std::shared_ptr<GameMap> map);
    void setResourcesPath(const std::string &path);
    void setSelectionMode(SelectionMode mode);
    MapTile *getCurrentMapTile();
    void setLastSelectedTexture(const std::string &name, int index);
    void setLastSelectedObject(const std::string &name, int index);
    void clearLastSelectedTexture();
    void clearLastSelectedObject();
    void stopAutoUpdate();
    void startAutoUpdate();
    void resetMapMovePosition();
    void updateGL();
    const std::vector<Texture>& getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
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
    void tileSelected(MapTile *tile, Point<> coord);
private:
    MapOpenGLWidget *m_glWidget;
    mapeditor::controllers::GLComponentController m_controller;
    MapTile *m_currentMapTile;
    std::string m_lastSelectedTextureName;
	std::string m_lastSelectedObjectName;
	int m_lastSelectedTextureIndex;
	int m_lastSelectedObjectIndex;
    void onTileClicked(int tileIndex);
    void onTileMouseReleaseEvent(std::vector<int> selectedTileIndexes);
    void addMoveDenyTrigger(const std::vector<int> &selectedTileIndexes, MapTileTriggerEvent event);
};
