#pragma once

#include <QtWidgets/QWidget>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "../mapOpenGLWidget.hpp"
#include "glComponentController.hpp"
#include "mapTile.hpp"
#include "mapView.hpp"
#include "monsterZoneDTO.hpp"
#include "point.hpp"
#include "selectionMode.hpp"


class MainForm_GLComponent : public QWidget {
    Q_OBJECT

 public:
    MainForm_GLComponent();
    void initializeUIObjects(MapOpenGLWidget *glWidget);
    void connectUIActions();
    const std::string &getResourcesPath() const;
    SelectionMode getSelectionMode() const;
    unsigned int getMapWidth() const;
    unsigned int getMapHeight() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void setResourcesPath(const std::string &path);
    void setSelectionMode(SelectionMode mode);
    void setMapView(MapView view);
    std::vector<thewarrior::models::MapTile *> getCurrentMapTiles();
    void setLastSelectedTexture(const std::string &name, int index);
    void setLastSelectedObject(const std::string &name, int index);
    void clearLastSelectedTexture();
    void clearLastSelectedObject();
    void setLastSelectedMonsterZone(int index);
    void clearLastSelectedMonsterZone();
    void stopAutoUpdate();
    void startAutoUpdate();
    void resetMapMovePosition();
    void updateGL();
    const std::vector<thewarrior::models::Texture>& getTextures() const;
    std::optional<std::reference_wrapper<const thewarrior::models::Texture>> getTextureByName(const std::string &name) const;
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
    std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    mapeditor::controllers::OptMonsterZoneDTOConst getMonsterZoneByName(const std::string &name) const;
    std::vector<std::string> getAlreadyUsedMonsterZoneNames() const;
 signals:
        void tileSelected(const std::vector<thewarrior::models::MapTile *> &tiles, thewarrior::models::Point<> coord);
        void tileUnselected();

 private:
    MapOpenGLWidget *m_glWidget;
    mapeditor::controllers::GLComponentController m_controller;
    std::vector<thewarrior::models::MapTile *> m_currentMapTiles;
    std::string m_lastSelectedTextureName;
    std::string m_lastSelectedObjectName;
    int m_lastSelectedTextureIndex;
    int m_lastSelectedObjectIndex;
    int m_lastSelectedMonsterZoneIndex;
    void onTileClicked(const std::set<int> &tileIndices, int, int);
    void onTileMouseReleaseEvent(std::set<int> selectedTileIndexes);
    void addMoveDenyTrigger(const std::set<int> &selectedTileIndexes,
            thewarrior::models::MapTileTriggerEvent event);
};
