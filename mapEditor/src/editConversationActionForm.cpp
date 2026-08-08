#include "fmt/format.h"
#include <QKeyEvent>
#include <QSignalBlocker>
#include <algorithm>
#include <functional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include "editConversationActionForm.hpp"
#include "errorMessage.hpp"
#include "glComponentController.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationActionFormController;
using mapeditor::controllers::GLComponentController;
using thewarrior::models::MerchantShopAction;
using thewarrior::models::RestRequestedAction;
using thewarrior::models::SellItemsAction;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

EditConversationActionForm::EditConversationActionForm(QWidget *parent,
                                                       const GLComponentController *glComponentController,
                                                       const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                                       const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds)
    : QDialog(parent),
    ui(Ui::editConversationActionFormClass()),
    m_controller(glComponentController, selectedConversationNode, alreadyUsedNodeIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    onActionTypeChanged();
    initializeComboBoxTransitionType();
    initializeMerchantInventoryComboBoxes();
    if (selectedConversationNode.has_value()) {
        auto transition = selectedConversationNode->getTransition();
        ui.lineEditId->setText(selectedConversationNode->getId().c_str());
        
        if (const auto *action = boost::get<thewarrior::models::ConversationAction>(&selectedConversationNode->getContent()); action != nullptr) {
            // Rest Requested Action
            const auto *restRequestedAction = boost::get<RestRequestedAction>(action);
            if (restRequestedAction != nullptr) {
                ui.radioButtonRestRequested->setChecked(true);
                ui.spinBoxRestRequestedGoldCost->setValue(static_cast<int>(restRequestedAction->goldCost));
                const auto &failureTransition = restRequestedAction->failureTransition;
                ui.comboBoxFailureTransitionType->setCurrentIndex(
                    static_cast<int>(failureTransition.getType()));
                if (failureTransition.getType() == ConversationNodeTransitionType::SpecificNode) {
                    ui.lineEditFailureNextNodeId->setText(
                        failureTransition.getNextNodeId().c_str());
                }
            }

            // Merchant Shop
            const auto *merchantShopAction = boost::get<MerchantShopAction>(action);
            if (merchantShopAction != nullptr) {
                ui.radioButtonMerchantShop->setChecked(true);
                ui.comboBoxMerchantShop->setCurrentText(merchantShopAction->merchantInventoryName.c_str());
            }
        
            //Sell Items
            const auto *sellItemsAction = boost::get<SellItemsAction>(action);
            if (sellItemsAction != nullptr) {
                ui.radioButtonSellItems->setChecked(true);
                ui.checkBoxOptionalMerchantShop->setChecked(sellItemsAction->merchantInventoryName.has_value());
                if (sellItemsAction->merchantInventoryName.has_value()) {
                    ui.comboBoxOptionalMerchantShop->setCurrentText(sellItemsAction->merchantInventoryName.value().c_str());
                }
                onCheckBoxOptionalMerchantShopChanged();
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
    connect(ui.radioButtonMerchantShop, &QRadioButton::toggled, this, &EditConversationActionForm::onActionTypeChanged);
    connect(ui.radioButtonSellItems, &QRadioButton::toggled, this, &EditConversationActionForm::onActionTypeChanged);
    connect(ui.checkBoxOptionalMerchantShop, &QCheckBox::stateChanged, this, &EditConversationActionForm::onCheckBoxOptionalMerchantShopChanged);
    connect(ui.comboBoxTransitionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationActionForm::onComboBoxTransitionTypeIndexChanged);
    connect(ui.comboBoxFailureTransitionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationActionForm::onComboBoxFailureTransitionTypeIndexChanged);
}

void EditConversationActionForm::initializeComboBoxTransitionType() {
    ui.comboBoxTransitionType->insertItem(0, "Next In Order");
    ui.comboBoxTransitionType->insertItem(1, "Stop");
    ui.comboBoxTransitionType->insertItem(2, "Specific Node");
    ui.comboBoxFailureTransitionType->insertItem(0, "Next In Order");
    ui.comboBoxFailureTransitionType->insertItem(1, "Stop");
    ui.comboBoxFailureTransitionType->insertItem(2, "Specific Node");
    ui.comboBoxFailureTransitionType->setCurrentIndex(
        static_cast<int>(ConversationNodeTransitionType::Stop));
}

void EditConversationActionForm::initializeMerchantInventoryComboBoxes() {
    std::ranges::for_each(m_controller.getMerchantInventoryNames(), [this](const auto &name) {
        ui.comboBoxMerchantShop->addItem(name.c_str());
    });
    std::ranges::for_each(m_controller.getMerchantInventoryNames(), [this](const auto &name) {
        ui.comboBoxOptionalMerchantShop->addItem(name.c_str());
    });
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
    bool merchantShopMode = ui.radioButtonMerchantShop->isChecked();
    bool sellItemsMode = ui.radioButtonSellItems->isChecked();

    if (!restRequestedMode && !progressStoryLineMode && !rewardMode && !merchantShopMode && !sellItemsMode) {
        ErrorMessage::show("The action type is required.");
        return;
    }

    if (progressStoryLineMode || rewardMode) {
        ErrorMessage::show("This action type has not yet been implemented.");
        return;
    }

    if (merchantShopMode) {
        if (ui.comboBoxMerchantShop->currentIndex() == -1) {
            ErrorMessage::show("This merchant inventory is required.");
            return;
        }
    }

    if (sellItemsMode) {
        if (ui.checkBoxOptionalMerchantShop->isChecked() && ui.comboBoxOptionalMerchantShop->currentIndex() == -1) {
            ErrorMessage::show("You must select a merchant inventory.");
            return;
        }
    }

    auto transitionType = static_cast<ConversationNodeTransitionType>(ui.comboBoxTransitionType->currentIndex());
    std::string nextNodeId = ui.lineEditNextNodeId->text().trimmed().toStdString();
    if (transitionType == ConversationNodeTransitionType::SpecificNode && nextNodeId.empty()) {
        ErrorMessage::show("The next node id is required.");
        return;
    }
    const auto failureTransitionType = static_cast<ConversationNodeTransitionType>(
        ui.comboBoxFailureTransitionType->currentIndex());
    const auto failureNextNodeId =
        ui.lineEditFailureNextNodeId->text().trimmed().toStdString();
    if (failureTransitionType == ConversationNodeTransitionType::SpecificNode &&
        failureNextNodeId.empty()) {
        ErrorMessage::show("The failure next node id is required.");
        return;
    }

    const auto createTransition = [](ConversationNodeTransitionType type,
                                     const ConversationNodeId &nodeId) {
        switch (type) {
            case ConversationNodeTransitionType::NextInOrder:
                return ConversationNodeTransition::nextInOrder();
            case ConversationNodeTransitionType::Stop:
                return ConversationNodeTransition::stop();
            case ConversationNodeTransitionType::SpecificNode:
                return ConversationNodeTransition::toNode(nodeId);
        }
        return ConversationNodeTransition::stop();
    };
    const auto transition = createTransition(transitionType, nextNodeId);
    const auto failureTransition =
        createTransition(failureTransitionType, failureNextNodeId);

    if (restRequestedMode) {
        m_result = ConversationNode(
            id,
            RestRequestedAction {
                .goldCost = static_cast<unsigned int>(ui.spinBoxRestRequestedGoldCost->value()),
                .failureTransition = failureTransition
            },
            transition
        );
    } else if (merchantShopMode) {
        m_result = ConversationNode(
            id,
            MerchantShopAction {
                .merchantInventoryName = ui.comboBoxMerchantShop->currentText().toStdString(),
            },
            transition
        );   
    } else if (sellItemsMode) {
        std::optional<std::string> optMerchantInventoryName = std::nullopt;
        if (ui.checkBoxOptionalMerchantShop->isChecked()) {
            optMerchantInventoryName = ui.comboBoxMerchantShop->currentText().toStdString();
        }
        m_result = ConversationNode(
            id,
            SellItemsAction {
                .merchantInventoryName = optMerchantInventoryName
            },
            transition
        );   
    }
    accept();
}

void EditConversationActionForm::onComboBoxTransitionTypeIndexChanged() {
    ui.labelNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
    ui.lineEditNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
}

void EditConversationActionForm::onComboBoxFailureTransitionTypeIndexChanged() {
    const bool isSpecificNode =
        ui.comboBoxFailureTransitionType->currentIndex() ==
        static_cast<int>(ConversationNodeTransitionType::SpecificNode);
    ui.labelFailureNextNodeId->setVisible(isSpecificNode);
    ui.lineEditFailureNextNodeId->setVisible(isSpecificNode);
}

void EditConversationActionForm::onActionTypeChanged() {
    // Rest Requested
    const bool restRequestedMode = ui.radioButtonRestRequested->isChecked();
    ui.labelRestRequestedConfig->setVisible(restRequestedMode);
    ui.labelRestRequestedConfig_2->setVisible(restRequestedMode);
    ui.labelRestRequestedGoldCost->setVisible(restRequestedMode);
    ui.spinBoxRestRequestedGoldCost->setVisible(restRequestedMode);
    
    // Merchant Shop
    const bool merchantShopMode = ui.radioButtonMerchantShop->isChecked();
    ui.labelMerchantShopConfig->setVisible(merchantShopMode);
    ui.labelMerchantShop->setVisible(merchantShopMode);
    ui.comboBoxMerchantShop->setVisible(merchantShopMode);
    
    // Sell Items
    const bool sellItemsMode = ui.radioButtonSellItems->isChecked();
    ui.labelSellItemsConfig->setVisible(sellItemsMode);
    ui.labelOptionalMerchantShop->setVisible(sellItemsMode);
    ui.checkBoxOptionalMerchantShop->setVisible(sellItemsMode);
    ui.comboBoxOptionalMerchantShop->setVisible(sellItemsMode && ui.checkBoxOptionalMerchantShop->isChecked());

    const bool fallibleActionMode = restRequestedMode || ui.radioButtonReward->isChecked();
    ui.labelFailureTransition->setVisible(fallibleActionMode);
    ui.comboBoxFailureTransitionType->setVisible(fallibleActionMode);
    ui.labelFailureNextNodeId->setVisible(
        fallibleActionMode &&
        ui.comboBoxFailureTransitionType->currentIndex() ==
            static_cast<int>(ConversationNodeTransitionType::SpecificNode));
    ui.lineEditFailureNextNodeId->setVisible(
        fallibleActionMode &&
        ui.comboBoxFailureTransitionType->currentIndex() ==
            static_cast<int>(ConversationNodeTransitionType::SpecificNode));
}

void EditConversationActionForm::onCheckBoxOptionalMerchantShopChanged() {
    ui.comboBoxOptionalMerchantShop->setVisible(ui.checkBoxOptionalMerchantShop->isChecked());
}
