#include "mainForm_GLComponent.hpp"
#include <algorithm>
#include <optional>
#include <stdexcept>
#include "errorMessage.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "point.hpp"

using commoneditor::ui::ErrorMessage;
using thewarrior::models::GameMap;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::Point;
using thewarrior::models::Texture;
using mapeditor::controllers::MonsterZoneDTO;
using mapeditor::controllers::OptMonsterZoneDTOConst;
using std::map;
using std::optional;
using std::set;
using std::string;
using std::vector;

MainForm_GLComponent::MainForm_GLComponent()
    : m_glWidget(nullptr),
    m_currentMapTiles({}),
    m_lastSelectedTextureName(""),
    m_lastSelectedObjectName(""),
    m_lastSelectedTextureIndex(-1),
    m_lastSelectedObjectIndex(-1),
    m_lastSelectedMonsterZoneIndex(-1) {
}

void MainForm_GLComponent::initializeUIObjects(MapOpenGLWidget *glWidget) {
    assert(glWidget);
    this->m_glWidget = glWidget;
    this->m_glWidget->setSelectionMode(SelectionMode::Select);
}

void MainForm_GLComponent::connectUIActions() {
    connect(this->m_glWidget,
            &MapOpenGLWidget::onTileClicked,
            this,
            &MainForm_GLComponent::onTileClicked);
    connect(this->m_glWidget,
            &MapOpenGLWidget::onTileMouseReleaseEvent,
            this,
            &MainForm_GLComponent::onTileMouseReleaseEvent);
}

const std::string &MainForm_GLComponent::getResourcesPath() const {
    return this->m_glWidget->getResourcesPath();
}

SelectionMode MainForm_GLComponent::getSelectionMode() const {
    return this->m_glWidget->getSelectionMode();
}

unsigned int MainForm_GLComponent::getMapWidth() const {
    return this->m_glWidget->getMapWidth();
}

unsigned int MainForm_GLComponent::getMapHeight() const {
    return this->m_glWidget->getMapHeight();
}

void MainForm_GLComponent::setCurrentMap(std::shared_ptr<GameMap> map) {
    this->m_controller.setCurrentMap(map);
    this->m_glWidget->setCurrentMap(map);
    m_currentMapTiles.clear();
    clearLastSelectedTexture();
    clearLastSelectedObject();
}

void MainForm_GLComponent::setResourcesPath(const std::string &path) {
    this->m_glWidget->setResourcesPath(path);
}

void MainForm_GLComponent::setSelectionMode(SelectionMode mode) {
    this->m_glWidget->setSelectionMode(mode);
}

void MainForm_GLComponent::setMapView(MapView view) {
    this->m_glWidget->setMapView(view);
}

std::vector<MapTile *> MainForm_GLComponent::getCurrentMapTiles() {
    return m_currentMapTiles;
}

void MainForm_GLComponent::setLastSelectedTexture(const std::string &name,
        int index) {
    this->m_lastSelectedTextureName = name;
    this->m_lastSelectedTextureIndex = index;
}

void MainForm_GLComponent::setLastSelectedObject(const std::string &name,
        int index) {
    this->m_lastSelectedObjectName = name;
    this->m_lastSelectedObjectIndex = index;
}

void MainForm_GLComponent::clearLastSelectedTexture() {
    m_lastSelectedTextureName = "";
    m_lastSelectedTextureIndex = -1;
}

void MainForm_GLComponent::clearLastSelectedObject() {
    m_lastSelectedObjectName = "";
    m_lastSelectedObjectIndex = -1;
}

void MainForm_GLComponent::setLastSelectedMonsterZone(int index) {
    m_lastSelectedMonsterZoneIndex = index;
}

void MainForm_GLComponent::clearLastSelectedMonsterZone() {
    m_lastSelectedMonsterZoneIndex = -1;
}

void MainForm_GLComponent::stopAutoUpdate() {
    m_glWidget->stopAutoUpdate();
}

void MainForm_GLComponent::startAutoUpdate() {
    m_glWidget->startAutoUpdate();
}

void MainForm_GLComponent::resetMapMovePosition() {
    m_glWidget->resetMapMovePosition();
}

void MainForm_GLComponent::updateGL() {
    m_glWidget->update();
}

const vector<Texture> &MainForm_GLComponent::getTextures() const {
    return m_controller.getMap()->getTextures();
}

optional<std::reference_wrapper<const Texture>> MainForm_GLComponent::getTextureByName(const std::string &name) const {
    return m_controller.getMap()->getTextureByName(name);
}

std::vector<std::string> MainForm_GLComponent::getAlreadyUsedTextureNames() const {
    return m_controller.getAlreadyUsedTextureNames();
}

bool MainForm_GLComponent::isTextureUsedInMap(const std::string &name) {
    return m_controller.isTextureUsedInMap(name);
}

void MainForm_GLComponent::reloadTextures() {
    m_glWidget->reloadTextures();
}

bool MainForm_GLComponent::isShrinkMapImpactAssignedTiles(int offsetLeft,
        int offsetTop,
        int offsetRight,
        int offsetBottom) const {
    return m_controller.isShrinkMapImpactAssignedTiles(offsetLeft,
            offsetTop,
            offsetRight,
            offsetBottom);
}

void MainForm_GLComponent::resizeMap(int offsetLeft,
        int offsetTop,
        int offsetRight,
        int offsetBottom) {
    m_controller.resizeMap(offsetLeft,
            offsetTop,
            offsetRight,
            offsetBottom);
}

void MainForm_GLComponent::onTileClicked(const std::set<int> &tileIndices, int, int) {
    if (m_glWidget->getSelectionMode() == SelectionMode::Select && tileIndices.size() != 0) {
        m_currentMapTiles.clear();
        m_currentMapTiles = m_controller.getMap()->getTilesForEditing(tileIndices);
        auto coord = [&tileIndices, this]() {
            return tileIndices.size() > 0 ?
                m_controller.getMap()->getCoordFromTileIndex(*tileIndices.begin()) :
                Point(0, 0);
        }();
        emit tileSelected(m_currentMapTiles, coord);
    } else {
        m_currentMapTiles.clear();
        emit tileUnselected();
    }
}

void MainForm_GLComponent::onTileMouseReleaseEvent(set<int> selectedTileIndexes) {
    // TODO: 0.3.3 To solve
    //if (m_glWidget->getSelectionMode() == SelectionMode::ApplyTexture) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setTextureName(m_lastSelectedTextureName);
            //m_currentMapTile->setTextureIndex(m_lastSelectedTextureIndex);
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::ApplyObject) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setObjectTextureName(m_lastSelectedObjectName);
            //m_currentMapTile->setObjectTextureIndex(m_lastSelectedObjectIndex);
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::EnableCanStep) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setCanPlayerSteppedOn(true);
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::DisableCanStep) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setCanPlayerSteppedOn(false);
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::BlockBorderLeft) {
        //addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveLeftPressed);
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::BlockBorderTop) {
        //addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveUpPressed);
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::BlockBorderRight) {
        //addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveRightPressed);
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::BlockBorderBottom) {
        //addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveDownPressed);
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::ClearBlockedBorders) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //for (const auto &trigger : m_currentMapTile->getTriggers()) {
                //if (trigger.getAction() == MapTileTriggerAction::DenyMove &&
                        //(trigger.getEvent() == MapTileTriggerEvent::MoveLeftPressed ||
                         //trigger.getEvent() == MapTileTriggerEvent::MoveUpPressed ||
                         //trigger.getEvent() == MapTileTriggerEvent::MoveRightPressed ||
                         //trigger.getEvent() == MapTileTriggerEvent::MoveDownPressed)) {
                    //m_currentMapTile->deleteTrigger(trigger);
                //}
            //}
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::ApplyMonsterZone) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setMonsterZoneIndex(m_lastSelectedMonsterZoneIndex);
        //}
    //} else if (m_glWidget->getSelectionMode() == SelectionMode::ClearMonsterZone) {
        //for (const int index : selectedTileIndexes) {
            //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
            //m_currentMapTile->setMonsterZoneIndex(-1);
        //}
    //}
}

void MainForm_GLComponent::addMoveDenyTrigger(const std::set<int> &selectedTileIndexes, MapTileTriggerEvent event) {
    // TODO: To solve
    //for (const int index : selectedTileIndexes) {
        //m_currentMapTile = &m_controller.getMap()->getTileForEditing(index);
        //if (!m_currentMapTile->findTrigger(event).has_value())
            //m_currentMapTile->addTrigger(MapTileTrigger(event,
                        //MapTileTriggerCondition::None,
                        //MapTileTriggerAction::DenyMove,
                        //map<string, string>()));
    //}
}

std::vector<MonsterZoneDTO> MainForm_GLComponent::getMonsterZones() const {
    return m_controller.getMonsterZones();
}

OptMonsterZoneDTOConst MainForm_GLComponent::getMonsterZoneByName(const std::string &name) const {
    return m_controller.getMonsterZoneByName(name);
}

std::vector<std::string> MainForm_GLComponent::getAlreadyUsedMonsterZoneNames() const {
    return m_controller.getAlreadyUsedMonsterZoneNames();
}
