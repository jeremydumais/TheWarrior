#include <fmt/format.h>
#include <QMenu>
#include <QAction>
#include <optional>
#include <string>
#include <vector>
#include "editConversationActionForm.hpp"
#include "editConversationChoiceForm.hpp"
#include "editConversationDialogueForm.hpp"
#include "editConversationScenarioForm.hpp"
#include "editConversationScenarioFormController.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationScenarioFormController;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
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

void EditConversationScenarioForm::initializeNodesTable() {
    ui.tableWidgetNodes->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetNodes->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    ui.tableWidgetNodes->setColumnWidth(0, 355);
    ui.tableWidgetNodes->setColumnWidth(1, 100);
    auto *addMenu = new QMenu(ui.pushButtonAddNode);
    addMenu->addAction("Dialogue", this, [this]() {
        EditConversationDialogueForm formTest(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        formTest.exec();
        refreshNodesTable();
    });
    addMenu->addAction("Choice", this, [this]() {
        EditConversationChoiceForm formTest(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        formTest.exec();
        refreshNodesTable();
    });
    addMenu->addAction("Action", this, [this]() {
        EditConversationActionForm formTest(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
        formTest.exec();
        refreshNodesTable();
    });
    ui.pushButtonAddNode->setMenu(addMenu);
}

void EditConversationScenarioForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonOKClick);
    connect(ui.pushButtonAddNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonAddNodeClick);
    connect(ui.pushButtonEditNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonEditNodeClick);
    connect(ui.tableWidgetNodes, &QTableWidget::itemDoubleClicked, this, &EditConversationScenarioForm::onTableWidgetNodesDoubleClicked);
    connect(ui.pushButtonDeleteNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonDeleteNodeClick);
    tableWidgetNodesKeyWatcher.installOn(ui.tableWidgetNodes);
    connect(&tableWidgetNodesKeyWatcher, &QTableWidgetKeyPressWatcher::keyPressed, this, &EditConversationScenarioForm::onTableWidgetNodesKeyPressEvent);
}

void EditConversationScenarioForm::refreshNodesTable() {
    ui.tableWidgetNodes->model()->removeRows(0, ui.tableWidgetNodes->rowCount());
    if (m_controller.isEditMode()) {
        int index = 0;
        for (const auto &node : m_controller.getNodes()) {
            ui.tableWidgetNodes->insertRow(index);
            ui.tableWidgetNodes->setItem(index, 0, new QTableWidgetItem(node.getId().c_str()));
            const auto contentType = ConversationNode::getContentTypeName(node.getContent());
            ui.tableWidgetNodes->setItem(index, 1, new QTableWidgetItem(contentType.c_str()));
            index++;
        }
    }
}

void EditConversationScenarioForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationScenarioForm::onPushButtonOKClick() {
    //TODO: Validate that all the nextNodeId exists
}

void EditConversationScenarioForm::onPushButtonAddNodeClick() {
    // std::vector<thewarrior::models::ConversationNode> nodes = {
    //     thewarrior::models::ConversationNode("node1",
    //         thewarrior::models::ConversationDialogue { .lines = {"This", "is", "a test"} },
    //         thewarrior::models::ConversationNodeTransition::toNode("test")
    //     )
    // };
    std::vector<thewarrior::models::ConversationNodeId> test;
    EditConversationDialogueForm formTest(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
    formTest.exec();

    // auto element = thewarrior::models::ConversationNode("node1",
    //     thewarrior::models::ConversationChoice { .prompt = "What would you choose?", .options = {{
    //         .text = "Choice1",
    //         .nextNodeId = "nextNode1"
    //     },{
    //         .text = "Choice2",
    //         .nextNodeId = "nextNode2"
    //     }}},
    //     thewarrior::models::ConversationNodeTransition::toNode("test"));
    // std::vector<thewarrior::models::ConversationNodeId> test {"node1", "node2"};
    // EditConversationChoiceForm formTest(this, element, test);
    // formTest.exec();

    // auto element = thewarrior::models::ConversationNode("node1",
    //     thewarrior::models::RestRequestedAction { .goldCost = 10 },
    //     thewarrior::models::ConversationNodeTransition::toNode("test"));
    // std::vector<std::string> ids = {"node1", "node2"};
    // EditConversationActionForm formTest(this, element, ids);
    // formTest.exec();
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
            EditConversationDialogueForm form(this, itemToEdit, alreadyUsedNodeIds);
            form.exec();
        } else if (boost::get<thewarrior::models::ConversationChoice>(&content) != nullptr) {
            EditConversationChoiceForm form(this, itemToEdit, alreadyUsedNodeIds);
            form.exec();
        } else if (boost::get<thewarrior::models::ConversationAction>(&content) != nullptr) {
            EditConversationActionForm form(this, itemToEdit, alreadyUsedNodeIds);
            form.exec();
        }
    }
}

void EditConversationScenarioForm::onPushButtonDeleteNodeClick() {
//TODO: Code this
}

void EditConversationScenarioForm::onTableWidgetNodesDoubleClicked(QTableWidgetItem *item) {
    if (item != nullptr) {
        onPushButtonEditNodeClick();
    }
}

void EditConversationScenarioForm::onTableWidgetNodesKeyPressEvent(int key, int, int) {
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