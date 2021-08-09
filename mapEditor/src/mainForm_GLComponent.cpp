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

const std::string &MainForm_GLComponent::getResourcesPath() const
{
	return this->glWidget->getResourcesPath();
}

SelectionMode MainForm_GLComponent::getSelectionMode() const
{
	return this->glWidget->getSelectionMode();
}

void MainForm_GLComponent::setResourcesPath(const std::string &path) 
{
	this->glWidget->setResourcesPath(path);
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

void MainForm_GLComponent::stopAutoUpdate() 
{
	glWidget->stopAutoUpdate();
}

void MainForm_GLComponent::startAutoUpdate() 
{
	glWidget->startAutoUpdate();
}

void MainForm_GLComponent::updateGL() 
{
	glWidget->updateGL();
}

const vector<Texture> &MainForm_GLComponent::getTextures() const
{
	return map->getTextures();
}

boost::optional<const Texture &> MainForm_GLComponent::getTextureByName(const std::string &name) const
{
	return map->getTextureByName(name);
}

std::vector<std::string> MainForm_GLComponent::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
	if (map != nullptr) {
    	transform(map->getTextures().begin(), map->getTextures().end(), back_inserter(alreadyUsedTextureNames),
                  [](Texture const& x) { return x.getName(); });
	}
	return alreadyUsedTextureNames;
}

bool MainForm_GLComponent::isTextureUsedInMap(const std::string &name) 
{
	for(const auto &row : map->getTiles()) {
		for (const auto &tile : row) {
			if (tile.getTextureName() == name || tile.getObjectTextureName() == name) {
				return true;
			}
		}
	}
	return false;
}

void MainForm_GLComponent::reloadTextures() 
{
    glWidget->reloadTextures();
}

bool MainForm_GLComponent::isShrinkMapImpactAssignedTiles(int offsetLeft, 
													      int offsetTop, 
														  int offsetRight, 
														  int offsetBottom) const
{
	return map->isShrinkMapImpactAssignedTiles(offsetLeft,
											   offsetTop,
											   offsetRight,
											   offsetBottom);
}

void MainForm_GLComponent::resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom) 
{
	map->resizeMap(offsetLeft,
				   offsetTop,
				   offsetRight,
				   offsetBottom);
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

