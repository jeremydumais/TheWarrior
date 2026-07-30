#include <fmt/format.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <QMenu>
#include <QAction>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>
#include "editConversationActionForm.hpp"
#include "editConversationChoiceForm.hpp"
#include "editConversationDialogueForm.hpp"
#include "editConversationScenarioForm.hpp"
#include "editConversationScenarioFormController.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationScenarioFormController;
using thewarrior::models::ConversationAction;
using thewarrior::models::ConversationChoice;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;
using thewarrior::models::RestRequestedAction;
using thewarrior::models::RewardAction;
using thewarrior::models::ConversationScenarioId;
using thewarrior::models::ConversationScenario;


EditConversationScenarioForm::EditConversationScenarioForm(QWidget *parent,
                         const std::string &resourcesPath,
                         const std::optional<ConversationScenario> &selectedConversationScenario,
                         const std::vector<ConversationScenarioId> &alreadyUsedScenarioIds)
    : QDialog(parent),
    ui(Ui::editConversationScenarioFormClass()),
    m_controller(resourcesPath, selectedConversationScenario, alreadyUsedScenarioIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    initializeNodesTable();
    connectUIActions();
    if (selectedConversationScenario.has_value()) {
        ui.lineEditId->setText(selectedConversationScenario->getId().c_str());
    }
    refreshNodesTable();
}

bool EditConversationScenarioForm::isEditMode() const {
    return m_controller.isEditMode();
}

const ConversationScenario &EditConversationScenarioForm::getResult() const {
    return m_result;   
}

void EditConversationScenarioForm::initializeNodesTable() {
    ui.tableWidgetNodes->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetNodes->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    ui.tableWidgetNodes->setHorizontalHeaderItem(2, new QTableWidgetItem("Transition"));
    ui.tableWidgetNodes->setColumnWidth(0, 155);
    ui.tableWidgetNodes->setColumnWidth(1, 100);
    ui.tableWidgetNodes->setColumnWidth(2, 200);
    auto *addMenu = new QMenu(ui.pushButtonAddNode);
    addMenu->addAction("Dialogue", this, [this]() {
        EditConversationDialogueForm formEditDialogue(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        if (formEditDialogue.exec() == QDialog::Accepted) {
            m_controller.addConversationNode(formEditDialogue.getResult());
            refreshNodesTable();
        }
    });
    addMenu->addAction("Choice", this, [this]() {
        EditConversationChoiceForm formEditChoice(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        if (formEditChoice.exec() == QDialog::Accepted) {
            m_controller.addConversationNode(formEditChoice.getResult());
            refreshNodesTable();
        }
    });
    addMenu->addAction("Action", this, [this]() {
        EditConversationActionForm formEditAction(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        if (formEditAction.exec() == QDialog::Accepted) {
            m_controller.addConversationNode(formEditAction.getResult());
            refreshNodesTable();
        }
    });
    ui.pushButtonAddNode->setMenu(addMenu);
}

void EditConversationScenarioForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonOKClick);
    connect(ui.pushButtonEditNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonEditNodeClick);
    connect(ui.tableWidgetNodes, &QTableWidget::itemDoubleClicked, this, &EditConversationScenarioForm::onTableWidgetNodesDoubleClicked);
    connect(ui.pushButtonDeleteNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonDeleteNodeClick);
    tableWidgetNodesKeyWatcher.installOn(ui.tableWidgetNodes);
    connect(&tableWidgetNodesKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &EditConversationScenarioForm::onTableWidgetNodesKeyPressEvent);
}

void EditConversationScenarioForm::refreshNodesTable() {
    ui.tableWidgetNodes->model()->removeRows(0, ui.tableWidgetNodes->rowCount());
    int index = 0;
    for (const auto &node : m_controller.getNodes()) {
        ui.tableWidgetNodes->insertRow(index);
        ui.tableWidgetNodes->setItem(index, 0, new QTableWidgetItem(node.getId().c_str()));
        const auto contentType = ConversationNode::getContentTypeName(node.getContent());
        ui.tableWidgetNodes->setItem(index, 1, new QTableWidgetItem(contentType.c_str()));
        const auto formatTransition = [](const ConversationNodeTransition &transition) {
            switch (transition.getType()) {
                case thewarrior::models::ConversationNodeTransitionType::NextInOrder:
                    return std::string("Next Node");
                case thewarrior::models::ConversationNodeTransitionType::Stop:
                    return std::string("Stop");
                case thewarrior::models::ConversationNodeTransitionType::SpecificNode:
                    return fmt::format("Specific Node({})", transition.getNextNodeId());
            }
            return std::string();
        };
        std::string transitionDisplayValue;
        if (const auto *choice = boost::get<ConversationChoice>(&node.getContent())) {
            for (const auto &option : choice->options) {
                if (!transitionDisplayValue.empty()) {
                    transitionDisplayValue += ", ";
                }
                transitionDisplayValue += fmt::format("{} ({})", option.text, option.nextNodeId);
            }
        } else if (const auto *action = boost::get<ConversationAction>(&node.getContent())) {
            transitionDisplayValue = formatTransition(node.getTransition());
            const ConversationNodeTransition *failureTransition = nullptr;
            if (const auto *reward = boost::get<RewardAction>(action)) {
                failureTransition = &reward->failureTransition;
            } else if (const auto *rest = boost::get<RestRequestedAction>(action)) {
                failureTransition = &rest->failureTransition;
            }
            if (failureTransition != nullptr) {
                transitionDisplayValue += fmt::format(", Failure: {}", formatTransition(*failureTransition));
            }
        } else {
            transitionDisplayValue = formatTransition(node.getTransition());
        }
        ui.tableWidgetNodes->setItem(index, 2, new QTableWidgetItem(transitionDisplayValue.c_str()));
        index++;
    }
}

void EditConversationScenarioForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationScenarioForm::onPushButtonOKClick() {
    std::string id = ui.lineEditId->text().trimmed().toStdString();
    if (id.empty()) {
        ErrorMessage::show("The scenario id is required.");
        return;
    }
    if (m_controller.isScenarioIdAlreadyUsed(id)) {
        ErrorMessage::show(fmt::format("The scenario id {} already exists in the list.", id));
        return;
    }

    if (ui.tableWidgetNodes->rowCount() == 0) {
        ErrorMessage::show("At least one node is required.");
        return;
    }

    // Dangling node-reference validation (SpecificNode transitions and ConversationChoiceOption::nextNodeId)
    const auto &nodes = m_controller.getNodes();
    std::unordered_set<ConversationNodeId> nodeIds;
    nodeIds.reserve(nodes.size());
    for (const auto &node : nodes) {
        nodeIds.insert(node.getId());
    }

    for (const auto &node : nodes) {
        const auto &transition = node.getTransition();
        if (transition.getType() == thewarrior::models::ConversationNodeTransitionType::SpecificNode &&
            !nodeIds.contains(transition.getNextNodeId())) {
            ErrorMessage::show(fmt::format(
                "Node {} references a next node id {} that does not exist.",
                node.getId(),
                transition.getNextNodeId()));
            return;
        }

        if (const auto *choice = boost::get<thewarrior::models::ConversationChoice>(&node.getContent())) {
            for (const auto &option : choice->options) {
                if (!nodeIds.contains(option.nextNodeId) && option.nextNodeId != "<stop>") {
                    ErrorMessage::show(fmt::format(
                        "Node {} references a next node id {} that does not exist.",
                        node.getId(),
                        option.nextNodeId));
                    return;
                }
            }
        }

        if (const auto *action = boost::get<thewarrior::models::ConversationAction>(&node.getContent())) {
            const ConversationNodeTransition *failureTransition = nullptr;
            if (const auto *reward =
                    boost::get<thewarrior::models::RewardAction>(action)) {
                failureTransition = &reward->failureTransition;
            } else if (const auto *rest =
                           boost::get<thewarrior::models::RestRequestedAction>(
                               action)) {
                failureTransition = &rest->failureTransition;
            }

            if (failureTransition != nullptr && 
                    failureTransition->getType() == ConversationNodeTransitionType::SpecificNode && 
                    !nodeIds.contains(failureTransition->getNextNodeId())) {
                ErrorMessage::show(fmt::format(
                    "Node {} references a failure node id {} that does not exist.",
                    node.getId(),
                    failureTransition->getNextNodeId()));
                return;
            }
        }
    }

    m_result = ConversationScenario(
        id,
        "",
        nodes
    );
    accept();
}

void EditConversationScenarioForm::onPushButtonEditNodeClick() {
    if (auto nodeId = getSelectedNodeId(); nodeId.has_value()) {
        auto itemToEdit = m_controller.getNodeById(nodeId.value());
        if (!itemToEdit.has_value()) {
            ErrorMessage::show(fmt::format("Unable to find the node with id {0}", nodeId.value()));
            return;
        }

        const auto alreadyUsedNodeIds = m_controller.getAlreadyUsedNodeIds();
        const auto &content = itemToEdit->getContent();
        if (boost::get<thewarrior::models::ConversationDialogue>(&content) != nullptr) {
            EditConversationDialogueForm formEditDialogue(this, itemToEdit, alreadyUsedNodeIds);
            if (formEditDialogue.exec() == QDialog::Accepted) {
                if (!m_controller.updateConversationNode(itemToEdit->getId(), formEditDialogue.getResult())) {
                    ErrorMessage::show(fmt::format("Unable to update the node with id {0}", itemToEdit->getId()));
                    return;
                }
                refreshNodesTable();
            }
        } else if (boost::get<thewarrior::models::ConversationChoice>(&content) != nullptr) {
            EditConversationChoiceForm formEditChoice(this, itemToEdit, alreadyUsedNodeIds);
            if (formEditChoice.exec() == QDialog::Accepted) {
                if (!m_controller.updateConversationNode(itemToEdit->getId(), formEditChoice.getResult())) {
                    ErrorMessage::show(fmt::format("Unable to update the node with id {0}", itemToEdit->getId()));
                    return;
                }
                refreshNodesTable();
            }
        } else if (boost::get<thewarrior::models::ConversationAction>(&content) != nullptr) {
            EditConversationActionForm formEditAction(this, itemToEdit, alreadyUsedNodeIds);
            if (formEditAction.exec() == QDialog::Accepted) {
                if (!m_controller.updateConversationNode(itemToEdit->getId(), formEditAction.getResult())) {
                    ErrorMessage::show(fmt::format("Unable to update the node with id {0}", itemToEdit->getId()));
                    return;
                }
                refreshNodesTable();
            }
        }
    }
}

void EditConversationScenarioForm::onPushButtonDeleteNodeClick() {
    if (auto nodeId = getSelectedNodeId(); nodeId.has_value()) {
        QMessageBox msgBox;
        msgBox.setText(fmt::format("Are you sure you want to delete the conversation node {}?", nodeId.value()).c_str());
        msgBox.setWindowTitle("Confirmation");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() == QMessageBox::Yes) {
            if (!m_controller.removeConversationNode(nodeId.value())) {
                ErrorMessage::show(m_controller.getLastError());
            }
            refreshNodesTable();
        }
    }
}

void EditConversationScenarioForm::onTableWidgetNodesDoubleClicked(QTableWidgetItem *item) {
    if (item != nullptr) {
        onPushButtonEditNodeClick();
    }
}

void EditConversationScenarioForm::onTableWidgetNodesKeyPressEvent(int key, int /*row*/, int /*column*/) {
    if (key == Qt::Key_Delete) {
        onPushButtonDeleteNodeClick();
    }
}

std::optional<ConversationNodeId> EditConversationScenarioForm::getSelectedNodeId() const {
    auto selectedRows = ui.tableWidgetNodes->selectionModel()->selectedRows();
    if (selectedRows.count() == 1) {
        return selectedRows[0].data().toString().toStdString();
    }
    return std::nullopt;
}
