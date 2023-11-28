#include "mainForm_GLComponent.hpp"
#include <algorithm>
#include <optional>
#include <stdexcept>
#include "errorMessage.hpp"
#include "glComponentController.hpp"
#include "mapTileDTO.hpp"
#include "monsterZoneDTO.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "pickerToolSelection.hpp"

using commoneditor::ui::ErrorMessage;
using thewarrior::models::GameMap;
using thewarrior::models::Texture;
using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MapTileDTO;
using mapeditor::controllers::MonsterZoneDTO;
using mapeditor::controllers::OptMonsterZoneDTOConst;
using std::optional;
using std::set;
using std::string;
using std::vector;

MainForm_GLComponent::MainForm_GLComponent()
    : m_glWidget(nullptr) {
}

GLComponentController *MainForm_GLComponent::getControllerPtr() {
    return &m_controller;
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
            &MapOpenGLWidget::onPickerToolTileSelected,
            this,
            &MainForm_GLComponent::onPickerToolTileSelected);
    connect(this->m_glWidget,
            &MapOpenGLWidget::onZoomChanged,
            this,
            &MainForm_GLComponent::onZoomChanged);
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

size_t MainForm_GLComponent::getHistoryCurrentIndex() const {
    return m_controller.getHistoryCurrentIndex();
}

size_t MainForm_GLComponent::getHistoryCount() const {
    return m_controller.getHistoryCount();
}

void MainForm_GLComponent::setCurrentMap(std::shared_ptr<GameMap> map) {
    this->m_controller.setCurrentMap(map);
    this->m_glWidget->setCurrentMap(map);
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

std::vector<MapTileDTO> MainForm_GLComponent::getCurrentMapTiles() {
    return m_controller.getSelectedMapTiles();
}

void MainForm_GLComponent::setLastSelectedTexture(const std::string &name,
        int index) {
    m_controller.setLastSelectedTexture(name, index);
}

void MainForm_GLComponent::setLastSelectedObject(const std::string &name,
        int index) {
    m_controller.setLastSelectedObject(name, index);
}

void MainForm_GLComponent::clearLastSelectedTexture() {
    m_controller.clearLastSelectedTexture();
}

void MainForm_GLComponent::clearLastSelectedObject() {
    m_controller.clearLastSelectedObject();
}

void MainForm_GLComponent::setLastSelectedMonsterZone(int index) {
    m_controller.setLastSelectedMonsterZone(index);
}

void MainForm_GLComponent::clearLastSelectedMonsterZone() {
    m_controller.clearLastSelectedMonsterZone();
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
        m_controller.selectTilesForEditing(tileIndices);
        emit tileSelected(m_controller.getSelectedMapTiles());
    } else {
        m_controller.unselectMapTiles();
        emit tileUnselected();
    }
}

void MainForm_GLComponent::onPickerToolTileSelected(const PickerToolSelection &selection) {
    emit pickerToolTileSelected(selection);
}

void MainForm_GLComponent::onZoomChanged(int zoomPercentage) {
    emit zoomChanged(zoomPercentage);
}

void MainForm_GLComponent::undo() {
    m_controller.undo();
    emit editHistoryChanged();
}

void MainForm_GLComponent::redo() {
    m_controller.redo();
    emit editHistoryChanged();
}

void MainForm_GLComponent::applyTexture() {
    m_controller.pushCurrentStateToHistory();
    m_controller.applyTexture();
    emit tilePropsChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::applyObject() {
    m_controller.pushCurrentStateToHistory();
    m_controller.applyObject();
    emit tilePropsChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::applyCanStep(bool value) {
    m_controller.pushCurrentStateToHistory();
    m_controller.applyCanStep(value);
    emit tilePropsChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::addMoveDenyTrigger(const std::string &event) {
    m_controller.pushCurrentStateToHistory();
    if (!m_controller.applyDenyZone(event)) {
        ErrorMessage::show(m_controller.getLastError());
    }
    emit tileTriggerChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::clearMoveDenyTriggers() {
    m_controller.pushCurrentStateToHistory();
    m_controller.clearDenyZones();
    emit tileTriggerChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::applyMonsterZone() {
    m_controller.pushCurrentStateToHistory();
    m_controller.applyMonsterZone();
    emit tileTriggerChanged();
    emit editHistoryChanged();
}

void MainForm_GLComponent::clearMonsterZone() {
    m_controller.pushCurrentStateToHistory();
    m_controller.clearMonsterZone();
    emit tileTriggerChanged();
    emit editHistoryChanged();
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
