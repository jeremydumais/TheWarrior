#include "mainForm_TileTabComponent.hpp"
#include "editMapTileTriggerForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "uiUtils.hpp"
#include <fmt/format.h>

MainForm_TileTabComponent::MainForm_TileTabComponent() 
	: m_glComponent(nullptr),
      m_labelTileCoordXY(nullptr),
      m_lineEditTexName(nullptr),
      m_spinBoxTexIndex(nullptr),
      m_lineEditObjTexName(nullptr),
      m_spinBoxObjTexIndex(nullptr),
      m_checkBoxTileCanSteppedOn(nullptr),
      m_checkBoxObjectAbovePlayer(nullptr),
	  m_checkBoxIsWallToClimb(nullptr),
	  m_listWidgetMapTileTriggers(nullptr),
	  m_pushButtonAddTileEvent(nullptr),
	  m_pushButtonEditTileEvent(nullptr),
	  m_pushButtonDeleteTileEvent(nullptr)
{  
}

void MainForm_TileTabComponent::initializeUIObjects(const MainForm_TileTabComponent_Objects &objects) 
{ 
    this->m_glComponent = objects.glComponent;
    this->m_labelTileCoordXY = objects.labelTileCoordXY;
	this->m_lineEditTexName = objects.lineEditTexName;
    this->m_spinBoxTexIndex = objects.spinBoxTexIndex;
	this->m_lineEditObjTexName = objects.lineEditObjTexName;
	this->m_spinBoxObjTexIndex = objects.spinBoxObjTexIndex;
	this->m_checkBoxTileCanSteppedOn = objects.checkBoxTileCanSteppedOn;
	this->m_checkBoxObjectAbovePlayer = objects.checkBoxObjectAbovePlayer;
	this->m_checkBoxIsWallToClimb = objects.checkBoxIsWallToClimb;
	this->m_listWidgetMapTileTriggers = objects.listWidgetMapTileTriggers;
	this->m_pushButtonAddTileEvent = objects.pushButtonAddTileEvent;
	this->m_pushButtonEditTileEvent = objects.pushButtonEditTileEvent;
	this->m_pushButtonDeleteTileEvent = objects.pushButtonDeleteTileEvent;
}

void MainForm_TileTabComponent::connectUIActions() 
{
	connect(m_glComponent, &MainForm_GLComponent::tileSelected, this, &MainForm_TileTabComponent::onTileSelected);
	connect(m_lineEditTexName, &QLineEdit::textChanged, this, &MainForm_TileTabComponent::onLineEditTexNameTextChanged);
	connect(m_spinBoxTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm_TileTabComponent::onSpinBoxTexIndexValueChanged);
	connect(m_lineEditObjTexName, &QLineEdit::textChanged, this, &MainForm_TileTabComponent::onLineEditObjTexNameTextChanged);
	connect(m_spinBoxObjTexIndex, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainForm_TileTabComponent::onSpinBoxObjTexIndexValueChanged);
	connect(m_checkBoxTileCanSteppedOn, &QCheckBox::stateChanged, this, &MainForm_TileTabComponent::onCheckBoxTileCanSteppedOnChanged);
	connect(m_checkBoxObjectAbovePlayer, &QCheckBox::stateChanged, this, &MainForm_TileTabComponent::onCheckBoxObjectAbovePlayerChanged);
	connect(m_checkBoxIsWallToClimb, &QCheckBox::stateChanged, this, &MainForm_TileTabComponent::onCheckBoxIsWallToClimbChanged);
	connect(m_pushButtonAddTileEvent, &QPushButton::clicked, this, &MainForm_TileTabComponent::onPushButtonAddTileEventClick);
	connect(m_pushButtonEditTileEvent, &QPushButton::clicked, this, &MainForm_TileTabComponent::onPushButtonEditTileEventClick);
	connect(m_pushButtonDeleteTileEvent, &QPushButton::clicked, this, &MainForm_TileTabComponent::onPushButtonDeleteTileEventClick);
}

void MainForm_TileTabComponent::reset() 
{
	m_labelTileCoordXY->clear();
	m_lineEditTexName->clear();
	m_spinBoxTexIndex->clear();
	m_lineEditObjTexName->clear();
	m_spinBoxObjTexIndex->clear();
	m_checkBoxTileCanSteppedOn->setChecked(false);
	m_checkBoxObjectAbovePlayer->setChecked(false);
	m_checkBoxIsWallToClimb->setChecked(false);
	refreshEventList(nullptr);
}

void MainForm_TileTabComponent::refreshEventList(MapTile *tile) 
{
	m_listWidgetMapTileTriggers->model()->removeRows(0, m_listWidgetMapTileTriggers->count());
	if (tile != nullptr) {
		int indexTrigger {0};
		for(const auto &trigger : tile->getTriggers()) {
			m_listWidgetMapTileTriggers->insertItem(indexTrigger, MapTileTriggerEventConverter::eventToString(trigger.getEvent()).c_str());
			indexTrigger++;
		}
	}
}

void MainForm_TileTabComponent::onTileSelected(MapTile *tile, Point coord) 
{
    m_labelTileCoordXY->setText(fmt::format("X: {0}, Y: {1}", coord.x(), coord.y()).c_str());
	m_lineEditTexName->setText(tile->getTextureName().c_str());
	m_spinBoxTexIndex->setValue(tile->getTextureIndex());
	m_lineEditObjTexName->setText(tile->getObjectTextureName().c_str());
	m_spinBoxObjTexIndex->setValue(tile->getObjectTextureIndex());
	m_checkBoxTileCanSteppedOn->setChecked(tile->canPlayerSteppedOn());
	m_checkBoxObjectAbovePlayer->setChecked(tile->getObjectAbovePlayer());
	m_checkBoxIsWallToClimb->setChecked(tile->getIsWallToClimb());
	refreshEventList(tile);
}


void MainForm_TileTabComponent::onLineEditTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureName(text.toStdString());
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onSpinBoxTexIndexValueChanged(int value) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setTextureIndex(value);
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onLineEditObjTexNameTextChanged(const QString &text) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureName(text.toStdString());
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onSpinBoxObjTexIndexValueChanged(int value) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectTextureIndex(value);
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onCheckBoxObjectAbovePlayerChanged(int state) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setObjectAbovePlayer(state == Qt::Checked);
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onCheckBoxTileCanSteppedOnChanged(int state) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setCanPlayerSteppedOn(state == Qt::Checked);
		m_glComponent->updateGL();
	}
}

void MainForm_TileTabComponent::onCheckBoxIsWallToClimbChanged(int state) 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		currentMapTile->setIsWallToClimb(state == Qt::Checked);
		m_glComponent->updateGL();
	}
}

boost::optional<MapTileTrigger &> MainForm_TileTabComponent::getSelectedTrigger() 
{
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (m_listWidgetMapTileTriggers->selectionModel()->hasSelection()) {
		//Find the selected trigger
		auto selectedItemName { m_listWidgetMapTileTriggers->selectionModel()->selectedRows()[0].data().toString().toStdString() };
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
	m_glComponent->stopAutoUpdate();
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		EditMapTileTriggerForm formEditMapTileTrigger(this, 
													m_glComponent->getResourcesPath(), 
													nullptr,
													currentMapTile->getTriggers());
		UIUtils::centerToScreen(&formEditMapTileTrigger);
		if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
			currentMapTile->addTrigger(formEditMapTileTrigger.getUpdatedTrigger());
			refreshEventList(m_glComponent->getCurrentMapTile());
		}
	}
	m_glComponent->startAutoUpdate();
}

void MainForm_TileTabComponent::onPushButtonEditTileEventClick() 
{
	m_glComponent->stopAutoUpdate();
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		//Find the selected event
		auto selectedMapTileTrigger { getSelectedTrigger() };
		if (selectedMapTileTrigger.has_value()) {
			EditMapTileTriggerForm formEditMapTileTrigger(this, 
														m_glComponent->getResourcesPath(), 
														&selectedMapTileTrigger.get(),
														currentMapTile->getTriggers());
			UIUtils::centerToScreen(&formEditMapTileTrigger);
			if (formEditMapTileTrigger.exec() == QDialog::Accepted) {
				if (!currentMapTile->updateTrigger(selectedMapTileTrigger.get(), formEditMapTileTrigger.getUpdatedTrigger())) {
					ErrorMessage::show("An error occurred while trying to update the selected trigger.");
				}
				refreshEventList(m_glComponent->getCurrentMapTile());
			}
		}
	}
	m_glComponent->startAutoUpdate();
}

void MainForm_TileTabComponent::onPushButtonDeleteTileEventClick() 
{
	m_glComponent->stopAutoUpdate();
	auto currentMapTile = m_glComponent->getCurrentMapTile();
	if (currentMapTile != nullptr) {
		//Find the selected event
		auto selectedMapTileTrigger { getSelectedTrigger() };
		if (selectedMapTileTrigger.has_value()) {
			QMessageBox msgBox;
			msgBox.setText(fmt::format("Are you sure you want to delete the trigger {0}?", 
									   MapTileTriggerEventConverter::eventToString(selectedMapTileTrigger->getEvent())).c_str());
			msgBox.setWindowTitle("Confirmation");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Cancel);
			if (msgBox.exec() == QMessageBox::Yes) {
				currentMapTile->deleteTrigger(selectedMapTileTrigger.get());
				refreshEventList(m_glComponent->getCurrentMapTile());
			}
		}
	}
	m_glComponent->startAutoUpdate();
}