#include "editMapTileTriggerForm.hpp"
#include "editTileActionChangeMapPropertiesForm.hpp"
#include "editTileActionOpenChestPropertiesForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerActionConverter.hpp"
#include "mapTileTriggerConditionConverter.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>

using namespace std;
using namespace commoneditor::ui;

EditMapTileTriggerForm::EditMapTileTriggerForm(QWidget *parent,
        const string &resourcesPath,
        const MapTileTrigger * const currentTrigger,
        const std::vector<MapTileTrigger> &allTriggers)
    : QDialog(parent),
    ui(Ui::editMapTileTriggerFormClass()),
    m_resourcesPath(resourcesPath),
    m_currentTrigger(currentTrigger),
    m_allTriggers(allTriggers),
    m_updatedTrigger(currentTrigger != nullptr ?
            *currentTrigger :
            MapTileTrigger()),
    m_isEditMode(currentTrigger != nullptr)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    connect(ui.pushButtonMapTileActionProperties,
            &QPushButton::clicked,
            this,
            &EditMapTileTriggerForm::onPushButtonTileActionPropertiesClick);
    connect(ui.pushButtonOK,
            &QPushButton::clicked,
            this,
            &EditMapTileTriggerForm::onPushButtonOK);
    connect(ui.pushButtonCancel,
            &QPushButton::clicked,
            this,
            &EditMapTileTriggerForm::reject);
    generateComboxItems();

    if (!m_isEditMode) {
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
    return m_updatedTrigger;
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
        EditTileActionChangeMapPropertiesForm formEdit(this,
                m_resourcesPath,
                m_updatedTrigger.getActionProperties());
        if (formEdit.exec() == QDialog::Accepted) {
            m_updatedTrigger.setActionProperties(formEdit.getUpdatedProperties());
        }
    }
    else if (ui.comboBoxAction->currentIndex() == static_cast<int>(MapTileTriggerAction::OpenChest)) {
        EditTileActionOpenChestPropertiesForm formEdit(this,
                m_updatedTrigger.getActionProperties());
        if (formEdit.exec() == QDialog::Accepted) {
            m_updatedTrigger.setActionProperties(formEdit.getUpdatedProperties());
        }
    }
}

void EditMapTileTriggerForm::onPushButtonOK()
{
    m_updatedTrigger.setEvent(static_cast<MapTileTriggerEvent>(ui.comboBoxEvent->currentIndex()));
    m_updatedTrigger.setCondition(static_cast<MapTileTriggerCondition>(ui.comboBoxCondition->currentIndex()));
    m_updatedTrigger.setAction(static_cast<MapTileTriggerAction>(ui.comboBoxAction->currentIndex()));
    if (m_updatedTrigger.getEvent() == MapTileTriggerEvent::None) {
        ErrorMessage::show("You must select an event!");
        return;
    }
    if (m_updatedTrigger.getAction() == MapTileTriggerAction::None) {
        ErrorMessage::show("You must select an action!");
        return;
    }
    auto selectedEvent = m_updatedTrigger.getEvent();
    //Check if the event is available (not already used by another trigger)
    if (std::find_if(m_allTriggers.begin(), m_allTriggers.end(), [&selectedEvent](const MapTileTrigger &trigger) {
                return trigger.getEvent() == selectedEvent; }) != m_allTriggers.end() &&
            (!m_isEditMode || (m_currentTrigger->getEvent() != m_updatedTrigger.getEvent()))) {
        ErrorMessage::show(fmt::format("The event {0} is already used.",
                    MapTileTriggerEventConverter::eventToString(m_updatedTrigger.getEvent())));
        return;
    }
    accept();
}
