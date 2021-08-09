#include "mainForm_TileTabComponent.hpp"
#include <fmt/format.h>

MainForm_TileTabComponent::MainForm_TileTabComponent() 
	: glComponent(nullptr),
      labelTileCoordXY(nullptr),
      lineEditTexName(nullptr),
      spinBoxTexIndex(nullptr),
      lineEditObjTexName(nullptr),
      spinBoxObjTexIndex(nullptr),
      checkBoxTileCanSteppedOn(nullptr),
      checkBoxObjectAbovePlayer(nullptr)
{  
}

void MainForm_TileTabComponent::initializeUIObjects(const MainForm_TileTabComponent_Objects &objects) 
{ 
    this->glComponent = objects.glComponent;
    this->labelTileCoordXY = objects.labelTileCoordXY;
	this->lineEditTexName = objects.lineEditTexName;
    this->spinBoxTexIndex = objects.spinBoxTexIndex;
	this->lineEditObjTexName = objects.lineEditObjTexName;
	this->spinBoxObjTexIndex = objects.spinBoxObjTexIndex;
	this->checkBoxTileCanSteppedOn = objects.checkBoxTileCanSteppedOn;
	this->checkBoxObjectAbovePlayer = objects.checkBoxObjectAbovePlayer;
}

void MainForm_TileTabComponent::connectUIActions() 
{
	connect(glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm_TileTabComponent::onTileSelected);
	connect(lineEditTexName, &QLineEdit::textChanged, this, &MainForm_TileTabComponent::onLineEditTexNameTextChanged);
	connect(spinBoxTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm_TileTabComponent::onSpinBoxTexIndexValueChanged);
	connect(lineEditObjTexName, &QLineEdit::textChanged, this, &MainForm_TileTabComponent::onLineEditObjTexNameTextChanged);
	connect(spinBoxObjTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm_TileTabComponent::onSpinBoxObjTexIndexValueChanged);
	connect(checkBoxTileCanSteppedOn, &QCheckBox::stateChanged, this, &MainForm_TileTabComponent::onCheckBoxTileCanSteppedOnChanged);
	connect(checkBoxObjectAbovePlayer, &QCheckBox::stateChanged, this, &MainForm_TileTabComponent::onCheckBoxObjectAbovePlayerChanged);
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


void MainForm_TileTabComponent::onLineEditTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureName(text.toStdString());
		glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onSpinBoxTexIndexValueChanged(int value) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureIndex(value);
		glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onLineEditObjTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureName(text.toStdString());
		glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onSpinBoxObjTexIndexValueChanged(int value) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureIndex(value);
		glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onCheckBoxObjectAbovePlayerChanged(int state) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectAbovePlayer(state == Qt::Checked);
		glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onCheckBoxTileCanSteppedOnChanged(int state) 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setCanPlayerSteppedOn(state == Qt::Checked);
		glComponent->updateGL();
	}
}