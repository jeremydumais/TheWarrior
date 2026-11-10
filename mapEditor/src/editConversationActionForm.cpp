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
using thewarrior::models::MonsterFightAction;
using thewarrior::models::ProgressStoryLineAction;
using thewarrior::models::RestRequestedAction;
using thewarrior::models::RewardAction;
using thewarrior::models::SellItemsAction;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

namespace {

enum class ActionType {
    MerchantShop,
    MonsterFight,
    ProgressStoryLine,
    RestRequested,
    Reward,
    SellItems
};

}

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
    initializeComboBoxActionType();
    initializeComboBoxTransitionType();
    initializeMerchantInventoryComboBoxes();
    connectUIActions();
    onActionTypeChanged();
    onComboBoxTransitionTypeIndexChanged();
    onComboBoxFailureTransitionTypeIndexChanged();
    if (selectedConversationNode.has_value()) {
        auto transition = selectedConversationNode->getTransition();
        ui.lineEditId->setText(selectedConversationNode->getId().c_str());
        
        if (const auto *action = boost::get<thewarrior::models::ConversationAction>(&selectedConversationNode->getContent()); action != nullptr) {
            if (boost::get<ProgressStoryLineAction>(action) != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::ProgressStoryLine));
            }

            if (boost::get<RewardAction>(action) != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::Reward));
            }

            if (boost::get<MonsterFightAction>(action) != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::MonsterFight));
            }

            if (boost::get<ProgressStoryLineAction>(action) != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::ProgressStoryLine));
            }

            // Rest Requested Action
            const auto *restRequestedAction = boost::get<RestRequestedAction>(action);
            if (restRequestedAction != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::RestRequested));
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
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::MerchantShop));
                ui.comboBoxMerchantShop->setCurrentText(merchantShopAction->merchantInventoryName.c_str());
            }

            // Monster Fight
            const auto *monsterFightAction = boost::get<MonsterFightAction>(action);
            if (monsterFightAction != nullptr) {
                ui.lineEditMonsterId->setText(monsterFightAction->monsterId.c_str());
            }

            // Progress Story Line
            const auto *progressStoryLineAction = boost::get<ProgressStoryLineAction>(action);
            if (progressStoryLineAction != nullptr) {
                ui.lineEditStoryId->setText(progressStoryLineAction->storyId.c_str());
            }  
        
            //Sell Items
            const auto *sellItemsAction = boost::get<SellItemsAction>(action);
            if (sellItemsAction != nullptr) {
                ui.comboBoxActionType->setCurrentIndex(static_cast<int>(ActionType::SellItems));
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
    connect(ui.comboBoxActionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationActionForm::onActionTypeChanged);
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

void EditConversationActionForm::initializeComboBoxActionType() {
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::MerchantShop), "Merchant Shop");
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::MonsterFight), "Monster Fight");
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::ProgressStoryLine), "Progress Story Line");
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::RestRequested), "Rest Request");
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::Reward), "Reward");
    ui.comboBoxActionType->insertItem(static_cast<int>(ActionType::SellItems), "Sell Items");
    ui.comboBoxActionType->setCurrentIndex(-1);
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

    const auto actionType = static_cast<ActionType>(ui.comboBoxActionType->currentIndex());
    const bool restRequestedMode = actionType == ActionType::RestRequested;
    const bool progressStoryLineMode = actionType == ActionType::ProgressStoryLine;
    const bool rewardMode = actionType == ActionType::Reward;
    const bool merchantShopMode = actionType == ActionType::MerchantShop;
    const bool monsterFightMode = actionType == ActionType::MonsterFight;
    const bool sellItemsMode = actionType == ActionType::SellItems;

    if (ui.comboBoxActionType->currentIndex() == -1) {
        ErrorMessage::show("The action type is required.");
        return;
    }

    if (rewardMode) {
        ErrorMessage::show("This action type has not yet been implemented.");
        return;
    }

    if (merchantShopMode) {
        if (ui.comboBoxMerchantShop->currentIndex() == -1) {
            ErrorMessage::show("This merchant inventory is required.");
            return;
        }
    }

    if (monsterFightMode) {
        if (ui.lineEditMonsterId->text().trimmed().isEmpty()) {
            ErrorMessage::show("The monster id is required.");
            return;
        }
    }

    if (progressStoryLineMode) {
        if (ui.lineEditStoryId->text().trimmed().isEmpty()) {
            ErrorMessage::show("The story id is required.");
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
    const bool fallibleActionMode = restRequestedMode || rewardMode;
    if (fallibleActionMode &&
        failureTransitionType == ConversationNodeTransitionType::SpecificNode &&
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
    } else if (monsterFightMode) {
        m_result = ConversationNode(
            id,
            MonsterFightAction {
                .monsterId = ui.lineEditMonsterId->text().trimmed().toStdString()
            },
            transition
        );   
    } else if (progressStoryLineMode) {
        m_result = ConversationNode(
            id,
            ProgressStoryLineAction {
                .storyId = ui.lineEditStoryId->text().trimmed().toStdString(),
            },
            transition
        );   
    } else if (sellItemsMode) {
        std::optional<std::string> optMerchantInventoryName = std::nullopt;
        if (ui.checkBoxOptionalMerchantShop->isChecked()) {
            optMerchantInventoryName = ui.comboBoxOptionalMerchantShop->currentText().toStdString();
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
    const auto actionType = static_cast<ActionType>(ui.comboBoxActionType->currentIndex());
    const bool fallibleActionMode =
        actionType == ActionType::RestRequested || actionType == ActionType::Reward;
    ui.labelFailureNextNodeId->setVisible(fallibleActionMode && isSpecificNode);
    ui.lineEditFailureNextNodeId->setVisible(fallibleActionMode && isSpecificNode);
}

void EditConversationActionForm::onActionTypeChanged() {
    const auto actionType = static_cast<ActionType>(ui.comboBoxActionType->currentIndex());

    // Rest Requested
    const bool restRequestedMode = actionType == ActionType::RestRequested;
    ui.labelRestRequestedConfig->setVisible(restRequestedMode);
    ui.labelRestRequestedConfig_2->setVisible(restRequestedMode);
    ui.labelRestRequestedGoldCost->setVisible(restRequestedMode);
    ui.spinBoxRestRequestedGoldCost->setVisible(restRequestedMode);
    
    // Merchant Shop
    const bool merchantShopMode = actionType == ActionType::MerchantShop;
    ui.labelMerchantShopConfig->setVisible(merchantShopMode);
    ui.labelMerchantShop->setVisible(merchantShopMode);
    ui.comboBoxMerchantShop->setVisible(merchantShopMode);
    
    // Sell Items
    const bool sellItemsMode = actionType == ActionType::SellItems;
    ui.labelSellItemsConfig->setVisible(sellItemsMode);
    ui.labelOptionalMerchantShop->setVisible(sellItemsMode);
    ui.checkBoxOptionalMerchantShop->setVisible(sellItemsMode);
    ui.comboBoxOptionalMerchantShop->setVisible(sellItemsMode && ui.checkBoxOptionalMerchantShop->isChecked());

    // Monster Fight
    const bool monsterFightMode = actionType == ActionType::MonsterFight;
    ui.labelMonsterFightConfig->setVisible(monsterFightMode);
    ui.labelMonsterId->setVisible(monsterFightMode);
    ui.lineEditMonsterId->setVisible(monsterFightMode);

    // Progress Story Line
    const bool progressStoryLineMode = actionType == ActionType::ProgressStoryLine;
    ui.labelProgressStoryLineConfig->setVisible(progressStoryLineMode);
    ui.labelStoryId->setVisible(progressStoryLineMode);
    ui.lineEditStoryId->setVisible(progressStoryLineMode);   

    const bool fallibleActionMode = restRequestedMode || actionType == ActionType::Reward;
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
    const auto actionType = static_cast<ActionType>(ui.comboBoxActionType->currentIndex());
    ui.comboBoxOptionalMerchantShop->setVisible(
        actionType == ActionType::SellItems && ui.checkBoxOptionalMerchantShop->isChecked());
}
