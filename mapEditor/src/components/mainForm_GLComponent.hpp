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
#include "mapView.hpp"
#include "mapTileDTO.hpp"
#include "monsterZoneDTO.hpp"
#include "pickerToolSelection.hpp"
#include "selectionMode.hpp"


class MainForm_GLComponent : public QWidget {
    Q_OBJECT

 public:
    MainForm_GLComponent();
    mapeditor::controllers::GLComponentController *getControllerPtr();
    void initializeUIObjects(MapOpenGLWidget *glWidget);
    void connectUIActions();
    const std::string &getResourcesPath() const;
    SelectionMode getSelectionMode() const;
    unsigned int getMapWidth() const;
    unsigned int getMapHeight() const;
    size_t getHistoryCurrentIndex() const;
    size_t getHistoryCount() const;
    bool isClipboardEmpty() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void setResourcesPath(const std::string &path);
    void setSelectionMode(SelectionMode mode);
    void setMapView(MapView view);
    std::vector<mapeditor::controllers::MapTileDTO> getCurrentMapTiles();
    bool isSelectedMapTiles() const;
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
    bool isUseOnlyOneMonsterZone() const;
    void clearEditHistory();
    void undo();
    void redo();
    void copySelectionInClipboard();
    void pasteClipboard();
    void applyTexture();
    void applyObject();
    void applyCanStep(bool value);
    void addMoveDenyTrigger(const std::string &event);
    void clearMoveDenyTriggers();
    void applyMonsterZone();
    void clearMonsterZone();
    bool setUseOnlyOneMonsterZone(bool value);

 signals:
    void tileSelected(std::vector<mapeditor::controllers::MapTileDTO> tiles);
    void tileUnselected();
    void tilePropsChanged();
    void tileTriggerChanged();
    void pickerToolTileSelected(const PickerToolSelection &selection);
    void editHistoryChanged();
    void clipboardChanged();
    void zoomChanged(int zoomPercentage);

 private:
    MapOpenGLWidget *m_glWidget;
    mapeditor::controllers::GLComponentController m_controller;
    void onTileClicked(const std::set<int> &tileIndices, int, int);
    void onPickerToolTileSelected(const PickerToolSelection &selection);
    void onZoomChanged(int zoomPercentage);
    void onClipboardPasted();
};
