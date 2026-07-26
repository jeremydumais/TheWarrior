#include <fmt/format.h>
#include <string>
#include <vector>
#include "editConversationDialogueForm.hpp"
#include "editConversationScenarioForm.hpp"
#include "editConversationScenarioFormController.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationScenarioFormController;
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
    refreshNodesTable();
}

bool EditConversationScenarioForm::isEditMode() const {
    return m_controller.isEditMode();
}

void EditConversationScenarioForm::initializeNodesTable() {
    ui.tableWidgetNodes->setHorizontalHeaderItem(0, new QTableWidgetItem("Id"));
    ui.tableWidgetNodes->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    ui.tableWidgetNodes->setColumnWidth(0, 375);
    ui.tableWidgetNodes->setColumnWidth(1, 80);
}

void EditConversationScenarioForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonOKClick);
    connect(ui.pushButtonAddNode, &QPushButton::clicked, this, &EditConversationScenarioForm::onPushButtonAddNodeClick);
}

void EditConversationScenarioForm::refreshNodesTable() {
}

void EditConversationScenarioForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationScenarioForm::onPushButtonOKClick() {
    //TODO: Validate that all the nextNodeId exists
}

void EditConversationScenarioForm::onPushButtonAddNodeClick() {
    std::vector<thewarrior::models::ConversationNode> nodes = {
        thewarrior::models::ConversationNode("node1",
            thewarrior::models::ConversationDialogue { .lines = {"This", "is", "a test"} },
            thewarrior::models::ConversationNodeTransition::toNode("test")
        )
    };
    std::vector<thewarrior::models::ConversationNodeId> test;
    EditConversationDialogueForm formTest(this, std::nullopt, m_controller.getAlreadyUsedNodeIds());
    formTest.exec();
}