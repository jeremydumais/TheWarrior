#include "mainForm_TileTabComponent.hpp"
#include "editMapTileTriggerForm.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "../utils/uiUtils.hpp"
#include <fmt/format.h>

MainForm_TileTabComponent::MainForm_TileTabComponent() 
	: glComponent(nullptr),
      labelTileCoordXY(nullptr),
      lineEditTexName(nullptr),
      spinBoxTexIndex(nullptr),
      lineEditObjTexName(nullptr),
      spinBoxObjTexIndex(nullptr),
      checkBoxTileCanSteppedOn(nullptr),
      checkBoxObjectAbovePlayer(nullptr),
	  listWidgetMapTileTriggers(nullptr),
	  pushButtonAddTileEvent(nullptr),
	  pushButtonEditTileEvent(nullptr),
	  pushButtonDeleteTileEvent(nullptr)
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
	this->listWidgetMapTileTriggers = objects.listWidgetMapTileTriggers;
	this->pushButtonAddTileEvent = objects.pushButtonAddTileEvent;
	this->pushButtonEditTileEvent = objects.pushButtonEditTileEvent;
	this->pushButtonDeleteTileEvent = objects.pushButtonDeleteTileEvent;
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
	connect(pushButtonAddTileEvent, &QPushButton::clicked, this, &MainForm_TileTabComponent::onPushButtonAddTileEventClick);
	connect(pushButtonEditTileEvent, &QPushButton::clicked, this, &MainForm_TileTabComponent::onPushButtonEditTileEventClick);
}

void MainForm_TileTabComponent::refreshEventList(MapTile *tile) 
{
	listWidgetMapTileTriggers->model()->removeRows(0, listWidgetMapTileTriggers->count());
	int indexTrigger {0};
	for(const auto &trigger : tile->getTriggers()) {
		listWidgetMapTileTriggers->insertItem(indexTrigger, MapTileTriggerEventConverter::eventToString(trigger.getEvent()).c_str());
		indexTrigger++;
	}
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
	refreshEventList(tile);
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

boost::optional<MapTileTrigger &> MainForm_TileTabComponent::getSelectedTrigger() 
{
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (listWidgetMapTileTriggers->selectionModel()->hasSelection()) {
		//Find the selected trigger
		auto selectedItemName { listWidgetMapTileTriggers->selectionModel()->selectedRows()[0].data().toString().toStdString() };
		auto parsedEvent { MapTileTriggerEventConverter::eventFromString(selectedItemName) };
		if (parsedEvent.has_value()) {
			return currentMapTile->findTrigger(parsedEvent.get());
		}
		else {
			return {};
		}
	}
	else {
		return {};
	}
}

void MainForm_TileTabComponent::onPushButtonAddTileEventClick()
{
	glComponent->stopAutoUpdate();
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		EditMapTileTriggerForm formEditMapTileTrigger(this, 
													glComponent->getResourcesPath(), 
													nullptr,
													currentMapTile->getTriggers());
		UIUtils::CenterToScreen(&formEditMapTileTrigger);
		if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
			currentMapTile->addTrigger(formEditMapTileTrigger.getUpdatedTrigger());
			refreshEventList(glComponent->getCurrentMapTile());
		}
	}
	glComponent->startAutoUpdate();
}

void MainForm_TileTabComponent::onPushButtonEditTileEventClick() 
{
	glComponent->stopAutoUpdate();
	auto currentMapTile = glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		//Find the selected event
		auto selectedMapTileTrigger { getSelectedTrigger() };
		if (selectedMapTileTrigger.has_value()) {
			EditMapTileTriggerForm formEditMapTileTrigger(this, 
														glComponent->getResourcesPath(), 
														&selectedMapTileTrigger.get(),
														currentMapTile->getTriggers());
			UIUtils::CenterToScreen(&formEditMapTileTrigger);
			if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
				//currentMapTile->updateTrigger(formEditMapTileTrigger.getUpdatedTrigger());
				refreshEventList(glComponent->getCurrentMapTile());
			}
		}
	}
	glComponent->startAutoUpdate();
}