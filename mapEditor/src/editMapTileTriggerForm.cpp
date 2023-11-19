#include "editMapTileTriggerForm.hpp"
#include <fmt/format.h>
#include <QFileDialog>
#include <QImageReader>
#include <algorithm>
#include <boost/optional/optional.hpp>
#include "editTileActionChangeMapPropertiesForm.hpp"
#include "editTileActionOpenChestPropertiesForm.hpp"
#include "errorMessage.hpp"
#include "mapTileTriggerActionConverter.hpp"
#include "mapTileTriggerConditionConverter.hpp"
#include "mapTileTriggerDTO.hpp"
#include "mapTileTriggerEventConverter.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::MapTileTriggerDTO;
using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MapTileTriggerEventConverter;
using thewarrior::models::MapTileTriggerConditionConverter;
using thewarrior::models::MapTileTriggerActionConverter;

EditMapTileTriggerForm::EditMapTileTriggerForm(QWidget *parent,
        const std::string &resourcesPath,
        const boost::optional<MapTileTriggerDTO> currentTrigger,
        const std::vector<MapTileTriggerDTO> &allTriggers)
    : QDialog(parent),
    ui(Ui::editMapTileTriggerFormClass()),
    m_controller(currentTrigger, allTriggers),
    m_resourcesPath(resourcesPath),
    m_isEditMode(currentTrigger.has_value()) {
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
    connect(ui.comboBoxEvent,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &EditMapTileTriggerForm::onComboBoxEventIndexChanged);
    connect(ui.comboBoxCondition,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &EditMapTileTriggerForm::onComboBoxConditionIndexChanged);
    connect(ui.comboBoxAction,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &EditMapTileTriggerForm::onComboBoxActionIndexChanged);

    generateComboxItems();

    if (!m_isEditMode) {
        this->setWindowTitle("Add map tile trigger");
    } else {
        this->setWindowTitle("Edit map tile trigger");
        ui.comboBoxEvent->setCurrentIndex(m_controller.getTriggerEventIndex());
        ui.comboBoxCondition->setCurrentIndex(m_controller.getTriggerConditionIndex());
        ui.comboBoxAction->setCurrentIndex(m_controller.getTriggerActionIndex());
    }
}

const MapTileTriggerDTO &EditMapTileTriggerForm::getUpdatedTrigger() const {
    return m_controller.getUpdatedTrigger();
}

void EditMapTileTriggerForm::generateComboxItems() {
    refreshComboBoxEvent();
    refreshComboBoxCondition();
    refreshComboBoxAction();
}

void EditMapTileTriggerForm::refreshComboBoxEvent() {
    ui.comboBoxEvent->model()->removeRows(0, ui.comboBoxEvent->count());
    int i = 0;
    for (const auto &eventStr : MapTileTriggerEventConverter::allEventsToString()) {
        ui.comboBoxEvent->insertItem(i, eventStr.c_str());
        i++;
    }
}

void EditMapTileTriggerForm::refreshComboBoxCondition() {
    ui.comboBoxCondition->model()->removeRows(0, ui.comboBoxCondition->count());
    int i = 0;
    for (const auto &conditionStr : MapTileTriggerConditionConverter::allConditionsToString()) {
        ui.comboBoxCondition->insertItem(i, conditionStr.c_str());
        i++;
    }
}

void EditMapTileTriggerForm::refreshComboBoxAction() {
    ui.comboBoxAction->model()->removeRows(0, ui.comboBoxAction->count());
    int i = 0;
    for (const auto &actionStr : MapTileTriggerActionConverter::allActionsToString()) {
        ui.comboBoxAction->insertItem(i, actionStr.c_str());
        i++;
    }
}

void EditMapTileTriggerForm::onPushButtonTileActionPropertiesClick() {
    if (ui.comboBoxAction->currentIndex() == static_cast<int>(MapTileTriggerAction::ChangeMap)) {
        EditTileActionChangeMapPropertiesForm formEdit(this,
                m_resourcesPath,
                m_controller.getUpdatedTrigger().actionProperties);
        if (formEdit.exec() == QDialog::Accepted) {
            m_controller.setActionProperties(formEdit.getUpdatedProperties());
        }
    } else if (ui.comboBoxAction->currentIndex() == static_cast<int>(MapTileTriggerAction::OpenChest)) {
        EditTileActionOpenChestPropertiesForm formEdit(this,
                m_controller.getUpdatedTrigger().actionProperties);
        if (formEdit.exec() == QDialog::Accepted) {
            m_controller.setActionProperties(formEdit.getUpdatedProperties());
        }
    }
}

void EditMapTileTriggerForm::onPushButtonOK() {
    if (!m_controller.isUpdatedTriggerValid()) {
        ErrorMessage::show(m_controller.getLastError());
        return;
    }
    //auto selectedEvent = m_updatedTrigger.getEvent();
    //// Check if the event is available (not already used by another trigger)
    //if (std::find_if(m_allTriggers.begin(), m_allTriggers.end(), [&selectedEvent](const MapTileTrigger &trigger) {
                //return trigger.getEvent() == selectedEvent; }) != m_allTriggers.end() &&
            //(!m_isEditMode || (m_currentTrigger->getEvent() != m_updatedTrigger.getEvent()))) {
        //ErrorMessage::show(fmt::format("The event {0} is already used.",
                    //MapTileTriggerEventConverter::eventToString(m_updatedTrigger.getEvent())));
        //return;
    //}
    //accept();
}

void EditMapTileTriggerForm::onComboBoxEventIndexChanged() {
    m_controller.setEvent(ui.comboBoxEvent->currentText().toStdString());
}

void EditMapTileTriggerForm::onComboBoxConditionIndexChanged() {
    m_controller.setCondition(ui.comboBoxCondition->currentText().toStdString());
}

void EditMapTileTriggerForm::onComboBoxActionIndexChanged() {
    m_controller.setAction(ui.comboBoxAction->currentText().toStdString());
}
