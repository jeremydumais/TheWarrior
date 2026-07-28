#include "fmt/format.h"
#include <QKeyEvent>
#include <QSignalBlocker>
#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "editConversationActionForm.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationActionFormController;
using thewarrior::models::RestRequestedAction;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

EditConversationActionForm::EditConversationActionForm(QWidget *parent,
                                                       const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                                       const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds)
    : QDialog(parent),
    ui(Ui::editConversationActionFormClass()),
    m_controller(selectedConversationNode, alreadyUsedNodeIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    onActionTypeChanged();
    initializeComboBoxTransitionType();
    if (selectedConversationNode.has_value()) {
        auto transition = selectedConversationNode->getTransition();
        ui.lineEditId->setText(selectedConversationNode->getId().c_str());
        
        // Rest Requested Action
        if (const auto *action = boost::get<thewarrior::models::ConversationAction>(&selectedConversationNode->getContent()); action != nullptr) {
            const auto *restRequestedAction = boost::get<RestRequestedAction>(action);
            if (restRequestedAction != nullptr) {
                ui.radioButtonRestRequested->setChecked(true);
                ui.spinBoxRestRequestedGoldCost->setValue(static_cast<int>(restRequestedAction->goldCost));
            }
        }

        // Transition
        ui.comboBoxTransitionType->setCurrentIndex(static_cast<int>(transition.getType()));
        if (transition.getType() == ConversationNodeTransitionType::SpecificNode) {
            ui.lineEditNextNodeId->setText(transition.getNextNodeId().c_str());
        }
    }
}

bool EditConversationActionForm::isEditMode() const {
    return m_controller.isEditMode();
}

const ConversationNode &EditConversationActionForm::getResult() const {
    return m_result;
}

void EditConversationActionForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationActionForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationActionForm::onPushButtonOKClick);
    connect(ui.radioButtonRestRequested, &QRadioButton::toggled, this, &EditConversationActionForm::onActionTypeChanged);
    connect(ui.radioButtonProgressStoryLine, &QRadioButton::toggled, this, &EditConversationActionForm::onActionTypeChanged);
    connect(ui.radioButtonReward, &QRadioButton::toggled, this, &EditConversationActionForm::onActionTypeChanged);
    connect(ui.comboBoxTransitionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationActionForm::onComboBoxTransitionTypeIndexChanged);
}

void EditConversationActionForm::initializeComboBoxTransitionType()
{
    ui.comboBoxTransitionType->insertItem(0, "Next In Order");
    ui.comboBoxTransitionType->insertItem(1, "Stop");
    ui.comboBoxTransitionType->insertItem(2, "Specific Node");
}

void EditConversationActionForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationActionForm::onPushButtonOKClick() {
    std::string id = ui.lineEditId->text().trimmed().toStdString();
    if (id.empty()) {
        ErrorMessage::show("The node id is required.");
        return;
    }
    if (m_controller.isNodeIdAlreadyUsed(id)) {
        ErrorMessage::show(fmt::format("The node id {} already exists in the list.", id));
        return;
    }

    bool restRequestedMode = ui.radioButtonRestRequested->isChecked();
    bool progressStoryLineMode = ui.radioButtonProgressStoryLine->isChecked();
    bool rewardMode = ui.radioButtonReward->isChecked();

    if (!restRequestedMode && !progressStoryLineMode && !rewardMode) {
        ErrorMessage::show("The action type is required.");
        return;
    }

    if (progressStoryLineMode || rewardMode) {
        ErrorMessage::show("This action type has not yet been implemented.");
        return;
    }

    auto transitionType = static_cast<ConversationNodeTransitionType>(ui.comboBoxTransitionType->currentIndex());
    std::string nextNodeId = ui.lineEditNextNodeId->text().trimmed().toStdString();
    if (transitionType == ConversationNodeTransitionType::SpecificNode && nextNodeId.empty()) {
        ErrorMessage::show("The next node id is required.");
        return;
    }
    ConversationNodeTransition transition;
    switch (transitionType) {
        case ConversationNodeTransitionType::NextInOrder:
            transition = ConversationNodeTransition::nextInOrder();
            break;
        case ConversationNodeTransitionType::Stop:
            transition = ConversationNodeTransition::stop();
            break;
        case ConversationNodeTransitionType::SpecificNode:
            transition = ConversationNodeTransition::toNode(nextNodeId);
            break;
    }
    m_result = ConversationNode(
        id,
        RestRequestedAction {
            .goldCost = static_cast<unsigned int>(ui.spinBoxRestRequestedGoldCost->value())
        },
        transition
    );
    accept();
}

void EditConversationActionForm::onComboBoxTransitionTypeIndexChanged() {
    ui.labelNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
    ui.lineEditNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
}

void EditConversationActionForm::onActionTypeChanged() {
    ui.labelRestRequestedConfig->setVisible(ui.radioButtonRestRequested->isChecked());
    ui.labelRestRequestedConfig_2->setVisible(ui.radioButtonRestRequested->isChecked());
    ui.labelRestRequestedGoldCost->setVisible(ui.radioButtonRestRequested->isChecked());
    ui.spinBoxRestRequestedGoldCost->setVisible(ui.radioButtonRestRequested->isChecked());
}
