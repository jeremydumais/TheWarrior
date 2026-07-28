#include "fmt/format.h"
#include <qlist.h>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include "editConversationDialogueForm.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationDialogueFormController;
using thewarrior::models::ConversationDialogue;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

EditConversationDialogueForm::EditConversationDialogueForm(QWidget *parent,
                                                           const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                                           const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds)
    : QDialog(parent),
    ui(Ui::editConversationDialogueFormClass()),
    m_controller(selectedConversationNode, alreadyUsedNodeIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    connectUIActions();
    initializeComboBoxTransitionType(); 
    if (selectedConversationNode.has_value()) {
        auto transition = selectedConversationNode->getTransition();
        ui.lineEditId->setText(selectedConversationNode->getId().c_str());
        QStringList qLines;
        const auto &dialogue = boost::get<ConversationDialogue>(selectedConversationNode->getContent());
        for (const auto &str : dialogue.lines) {
            qLines.append(QString::fromStdString(str));
        }
        ui.plainTextEditDialogue->setPlainText(qLines.join("\n"));
        ui.comboBoxTransitionType->setCurrentIndex(static_cast<int>(transition.getType()));
        if (transition.getType() == ConversationNodeTransitionType::SpecificNode) {
            ui.lineEditNextNodeId->setText(transition.getNextNodeId().c_str());
        }
    }
}

bool EditConversationDialogueForm::isEditMode() const {
    return m_controller.isEditMode();
}

void EditConversationDialogueForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationDialogueForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationDialogueForm::onPushButtonOKClick);
    connect(ui.comboBoxTransitionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationDialogueForm::onComboBoxTransitionTypeIndexChanged);
}

void EditConversationDialogueForm::initializeComboBoxTransitionType()
{
    ui.comboBoxTransitionType->insertItem(0, "Next In Order");
    ui.comboBoxTransitionType->insertItem(1, "Stop");
    ui.comboBoxTransitionType->insertItem(2, "Specific Node");
}

void EditConversationDialogueForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationDialogueForm::onPushButtonOKClick() {
    std::string id = ui.lineEditId->text().trimmed().toStdString();
    if (id.empty()) {
        ErrorMessage::show("The node id is required.");
        return;
    }
    if (m_controller.isNodeIdAlreadyUsed(id)) {
        ErrorMessage::show(fmt::format("The node id {} already exists in the list.", id));
        return;
    }
    auto dialogue = ui.plainTextEditDialogue->toPlainText().trimmed();
    if (dialogue.isEmpty()) {
        ErrorMessage::show("The dialogue is required.");
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
        ConversationDialogue { .lines = convertPlainTextToLines(dialogue) },
        transition
    );
    accept();
}

void EditConversationDialogueForm::onComboBoxTransitionTypeIndexChanged() {
    ui.labelNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
    ui.lineEditNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
}

std::vector<std::string> EditConversationDialogueForm::convertPlainTextToLines(const QString &text) {
    std::vector<std::string> result {};
    QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        result.push_back(line.trimmed().toStdString());
    }
    return result;
}
