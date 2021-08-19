#include "mainForm_GLComponent.hpp"

using namespace std;

MainForm_GLComponent::MainForm_GLComponent() 
	: glWidget(nullptr),
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

const std::string &MainForm_GLComponent::getResourcesPath() const
{
	return this->glWidget->getResourcesPath();
}

SelectionMode MainForm_GLComponent::getSelectionMode() const
{
	return this->glWidget->getSelectionMode();
}

unsigned int MainForm_GLComponent::getMapWidth() const
{
	return this->glWidget->getMapWidth();
}

unsigned int MainForm_GLComponent::getMapHeight() const
{
	return this->glWidget->getMapHeight();
}

void MainForm_GLComponent::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    this->controller.setCurrentMap(map);
	this->glWidget->setCurrentMap(map); 
	currentMapTile = nullptr;
	clearLastSelectedTexture();
	clearLastSelectedObject();
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

void MainForm_GLComponent::resetMapMovePosition() 
{
	glWidget->resetMapMovePosition();
}

void MainForm_GLComponent::updateGL() 
{
	glWidget->updateGL();
}

const vector<Texture> &MainForm_GLComponent::getTextures() const
{
	return controller.getMap()->getTextures();
}

boost::optional<const Texture &> MainForm_GLComponent::getTextureByName(const std::string &name) const
{
	return controller.getMap()->getTextureByName(name);
}

std::vector<std::string> MainForm_GLComponent::getAlreadyUsedTextureNames() const
{
	return controller.getAlreadyUsedTextureNames();
}

bool MainForm_GLComponent::isTextureUsedInMap(const std::string &name) 
{
	return controller.isTextureUsedInMap(name);
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
	return controller.isShrinkMapImpactAssignedTiles(offsetLeft,
											   		 offsetTop,
											   		 offsetRight,
											   		 offsetBottom);
}

void MainForm_GLComponent::resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom) 
{
	controller.resizeMap(offsetLeft,
				   		 offsetTop,
				   		 offsetRight,
				   		 offsetBottom);
}

void MainForm_GLComponent::onTileClicked(int tileIndex) 
{
	if (glWidget->getSelectionMode() == SelectionMode::Select && tileIndex != -1) {
		currentMapTile = nullptr;
		auto tempTile { &controller.getMap()->getTileForEditing(tileIndex) };
		auto coord { controller.getMap()->getCoordFromTileIndex(tileIndex) };
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
			currentMapTile = &controller.getMap()->getTileForEditing(index);
			currentMapTile->setTextureName(lastSelectedTextureName);
			currentMapTile->setTextureIndex(lastSelectedTextureIndex);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::ApplyObject) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &controller.getMap()->getTileForEditing(index);
			currentMapTile->setObjectTextureName(lastSelectedObjectName);
			currentMapTile->setObjectTextureIndex(lastSelectedObjectIndex);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::EnableCanStep) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &controller.getMap()->getTileForEditing(index);
			currentMapTile->setCanPlayerSteppedOn(true);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::DisableCanStep) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &controller.getMap()->getTileForEditing(index);
			currentMapTile->setCanPlayerSteppedOn(false);
		}
	}
	else if (glWidget->getSelectionMode() == SelectionMode::BlockBorderLeft) {
		addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveLeftPressed);
	}	
	else if (glWidget->getSelectionMode() == SelectionMode::BlockBorderTop) {
		addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveUpPressed);
	}	
	else if (glWidget->getSelectionMode() == SelectionMode::BlockBorderRight) {
		addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveRightPressed);
	}	
	else if (glWidget->getSelectionMode() == SelectionMode::BlockBorderBottom) {
		addMoveDenyTrigger(selectedTileIndexes, MapTileTriggerEvent::MoveDownPressed);
	}
	else if (glWidget->getSelectionMode() == SelectionMode::ClearBlockedBorders) {
		for(const int index : selectedTileIndexes) {
			currentMapTile = &controller.getMap()->getTileForEditing(index);
			for(const auto &trigger : currentMapTile->getTriggers()) {
				if (trigger.getAction() == MapTileTriggerAction::DenyMove && 
					(trigger.getEvent() == MapTileTriggerEvent::MoveLeftPressed || 
					 trigger.getEvent() == MapTileTriggerEvent::MoveUpPressed || 
					 trigger.getEvent() == MapTileTriggerEvent::MoveRightPressed || 
					 trigger.getEvent() == MapTileTriggerEvent::MoveDownPressed)) {
					currentMapTile->deleteTrigger(trigger);
				}
			}
		}
	}

}

void MainForm_GLComponent::addMoveDenyTrigger(const std::vector<int> &selectedTileIndexes, MapTileTriggerEvent event) 
{
	for(const int index : selectedTileIndexes) {
		currentMapTile = &controller.getMap()->getTileForEditing(index);
		if (!currentMapTile->findTrigger(event).has_value())
			currentMapTile->addTrigger(MapTileTrigger(event,
													  MapTileTriggerCondition::None,
													  MapTileTriggerAction::DenyMove,
													  map<string, string>()));
	}
}

