#include "mainForm_TileTabComponent.hpp"
#include <fmt/format.h>

MainForm_TileTabComponent::MainForm_TileTabComponent() 
{  
}

void MainForm_TileTabComponent::initializeUIObjects(const MainForm_TileTabComponent_Objects &objects) 
{ 
    this->labelTileCoordXY = objects.labelTileCoordXY;
	this->lineEditTexName = objects.lineEditTexName;
    this->spinBoxTexIndex = objects.spinBoxTexIndex;
	this->lineEditObjTexName = objects.lineEditObjTexName;
	this->spinBoxObjTexIndex = objects.spinBoxObjTexIndex;
	this->checkBoxTileCanSteppedOn = objects.checkBoxTileCanSteppedOn;
	this->checkBoxObjectAbovePlayer = objects.checkBoxObjectAbovePlayer;
}

void MainForm_TileTabComponent::onTileSelected(MapTile *tile, Point coord) 
{
    labelTileCoordXY->setText(fmt::format("X: {0}, Y: {1}", coord.x(), coord.y()).c_str());
	lineEditTexName->setText(tile->getTextureName().c_str());
	spinBoxTexIndex->setValue(tile->getTextureIndex());
	lineEditObjTexName->setText(tile->getObjectTextureName().c_str());
	spinBoxObjTexIndex->setValue(tile->getObjectTextureIndex());
	checkBoxTileCanSteppedOn->setChecked(tile->canPlayerSteppedOn());
	checkBoxObjectAbovePlayer->setChecked(tile->getObjectAbovePlayer());
		
		//ui.comboBoxTileTrigger->setCurrentIndex(static_cast<int>(tempTile->getTrigger()));
		//ui.comboBoxTileCondition->setCurrentIndex(static_cast<int>(tempTile->getCondition()));
		//ui.comboBoxTileAction->setCurrentIndex(static_cast<int>(tempTile->getAction()));
}
