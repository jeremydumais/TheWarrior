#include "mainForm_GLComponent.hpp"

using namespace std;

MainForm_GLComponent::MainForm_GLComponent() 
	: glWidget(nullptr),
      map(nullptr),
      currentMapTile(nullptr),
      lastSelectedTextureName(""),
	  lastSelectedObjectName(""),
	  lastSelectedTextureIndex(-1),
	  lastSelectedObjectIndex(-1)
{  
}

void MainForm_GLComponent::initializeUIObjects(MapOpenGLWidget *glWidget) 
{
    assert(glWidget);
    this->glWidget = glWidget;
    this->glWidget->setSelectionMode(SelectionMode::Select);
}

void MainForm_GLComponent::connectUIActions() 
{
    connect(this->glWidget, &MapOpenGLWidget::onTileClicked, this, &MainForm_GLComponent::onTileClicked);
	connect(this->glWidget, &MapOpenGLWidget::onTileMouseReleaseEvent, this, &MainForm_GLComponent::onTileMouseReleaseEvent);
}

void MainForm_GLComponent::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    this->map = map;
	this->glWidget->setCurrentMap(map); 
}

void MainForm_GLComponent::setSelectionMode(SelectionMode mode) 
{
    this->glWidget->setSelectionMode(mode);
}

MapTile* MainForm_GLComponent::getCurrentMapTile() 
{
    return currentMapTile;
}

void MainForm_GLComponent::setLastSelectedTexture(const std::string &name, int index) 
{
    this->lastSelectedTextureName = name;
    this->lastSelectedTextureIndex = index;
}

void MainForm_GLComponent::setLastSelectedObject(const std::string &name, int index) 
{
    this->lastSelectedObjectName = name;
    this->lastSelectedObjectIndex = index;
}

void MainForm_GLComponent::clearLastSelectedTexture() 
{
    lastSelectedTextureName = "";
	lastSelectedTextureIndex = -1;
}

void MainForm_GLComponent::clearLastSelectedObject() 
{
    lastSelectedObjectName = "";
	lastSelectedObjectIndex = -1;
}

void MainForm_GLComponent::onTileClicked(int tileIndex) 
{
	if (glWidget->getSelectionMode() == SelectionMode::Select && tileIndex != -1) {
		currentMapTile = nullptr;
		auto tempTile { &map->getTileForEditing(tileIndex) };
		auto coord { map->getCoordFromTileIndex(tileIndex) };
        emit tileSelected(tempTile, coord);
		currentMapTile = tempTile;
	}
	else {
		currentMapTile = nullptr;
	}
}

void MainForm_GLComponent::onTileMouseReleaseEvent(vector<int> selectedTileIndexes) 
{
	if (glWidget->getSelectionMode() == SelectionMode::ApplyTexture) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setTextureName(lastSelectedTextureName);
			currentMapTile->setTextureIndex(lastSelectedTextureIndex);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::ApplyObject) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setObjectTextureName(lastSelectedObjectName);
			currentMapTile->setObjectTextureIndex(lastSelectedObjectIndex);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::EnableCanStep) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setCanPlayerSteppedOn(true);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::DisableCanStep) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &map->getTileForEditing(index);
			currentMapTile->setCanPlayerSteppedOn(false);
		}
	}
}

