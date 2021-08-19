#include "editMapTileTriggerForm.hpp"
#include "editTileActionChangeMapPropertiesForm.hpp"
#include "editTileActionOpenChestPropertiesForm.hpp"
#include "utils/errorMessage.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerActionConverter.hpp"
#include "mapTileTriggerConditionConverter.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>

using namespace std;

EditMapTileTriggerForm::EditMapTileTriggerForm(QWidget *parent,
											   const string &resourcesPath,
											   const MapTileTrigger * const currentTrigger,
											   const std::vector<MapTileTrigger> &allTriggers)
	: QDialog(parent),
	  ui(Ui::editMapTileTriggerFormClass()),
	  resourcesPath(resourcesPath),
	  currentTrigger(currentTrigger),
	  allTriggers(allTriggers),
	  updatedTrigger(currentTrigger != nullptr ? *currentTrigger : MapTileTrigger()),
	  isEditMode(currentTrigger != nullptr)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/MapEditor Icon.png"));
	connect(ui.pushButtonMapTileActionProperties, &QPushButton::clicked, this, &EditMapTileTriggerForm::onPushButtonTileActionPropertiesClick);
	connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditMapTileTriggerForm::onPushButtonOK);
	connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditMapTileTriggerForm::reject);
	generateComboxItems();
	
	if (!isEditMode) {
		this->setWindowTitle("Add map tile trigger");
	}
	else {
		this->setWindowTitle("Edit map tile trigger");
		ui.comboBoxEvent->setCurrentIndex(static_cast<int>(currentTrigger->getEvent()));
		ui.comboBoxCondition->setCurrentIndex(static_cast<int>(currentTrigger->getCondition()));
		ui.comboBoxAction->setCurrentIndex(static_cast<int>(currentTrigger->getAction()));
	}
}

const MapTileTrigger &EditMapTileTriggerForm::getUpdatedTrigger() const
{
	return updatedTrigger;
}

void EditMapTileTriggerForm::generateComboxItems() 
{
	refreshComboBoxEvent();
	refreshComboBoxCondition();
	refreshComboBoxAction();
}

void EditMapTileTriggerForm::refreshComboBoxEvent() 
{
	ui.comboBoxEvent->model()->removeRows(0, ui.comboBoxEvent->count());
	int i = 0;
	for (const auto &eventStr : MapTileTriggerEventConverter::allEventsToString()) {
		ui.comboBoxEvent->insertItem(i, eventStr.c_str());
		i++;
	}
}

void EditMapTileTriggerForm::refreshComboBoxCondition() 
{
	ui.comboBoxCondition->model()->removeRows(0, ui.comboBoxCondition->count());
	int i = 0;
	for (const auto &conditionStr : MapTileTriggerConditionConverter::allConditionsToString()) {
		ui.comboBoxCondition->insertItem(i, conditionStr.c_str());
		i++;
	}
}

void EditMapTileTriggerForm::refreshComboBoxAction() 
{
	ui.comboBoxAction->model()->removeRows(0, ui.comboBoxAction->count());
	int i = 0;
	for (const auto &actionStr : MapTileTriggerActionConverter::allActionsToString()) {
		ui.comboBoxAction->insertItem(i, actionStr.c_str());
		i++;
	}
}

void EditMapTileTriggerForm::onPushButtonTileActionPropertiesClick() 
{
	if (ui.comboBoxAction->currentIndex() == static_cast<int>(MapTileTriggerAction::ChangeMap)) {
		EditTileActionChangeMapPropertiesForm formEditActionProperties(this, 
																	   resourcesPath, 
																	   updatedTrigger.getActionProperties());
		if (formEditActionProperties.exec() == QDialog::Accepted) {
			updatedTrigger.setActionProperties(formEditActionProperties.getUpdatedProperties());
		}
	}
	else if (ui.comboBoxAction->currentIndex() == static_cast<int>(MapTileTriggerAction::OpenChest)) {
		EditTileActionOpenChestPropertiesForm formEditActionProperties(this,
																	   updatedTrigger.getActionProperties());
		if (formEditActionProperties.exec() == QDialog::Accepted) {
			updatedTrigger.setActionProperties(formEditActionProperties.getUpdatedProperties());
		}
	}
}

void EditMapTileTriggerForm::onPushButtonOK() 
{
	updatedTrigger.setEvent(static_cast<MapTileTriggerEvent>(ui.comboBoxEvent->currentIndex()));
	updatedTrigger.setCondition(static_cast<MapTileTriggerCondition>(ui.comboBoxCondition->currentIndex()));
	updatedTrigger.setAction(static_cast<MapTileTriggerAction>(ui.comboBoxAction->currentIndex()));
	if (updatedTrigger.getEvent() == MapTileTriggerEvent::None) {
		ErrorMessage::show("You must select an event!");
		return;
	}
	if (updatedTrigger.getAction() == MapTileTriggerAction::None) {
		ErrorMessage::show("You must select an action!");
		return;
	}
	auto selectedEvent = updatedTrigger.getEvent();
	//Check if the event is available (not already used by another trigger)
	if (std::find_if(allTriggers.begin(), allTriggers.end(), [&selectedEvent](const MapTileTrigger &trigger) {
			return trigger.getEvent() == selectedEvent; }) != allTriggers.end() && 
		(!isEditMode || (currentTrigger->getEvent() != updatedTrigger.getEvent()))) {
		ErrorMessage::show(fmt::format("The event {0} is already used.", MapTileTriggerEventConverter::eventToString(updatedTrigger.getEvent())));
		return;
	}
	accept();
}