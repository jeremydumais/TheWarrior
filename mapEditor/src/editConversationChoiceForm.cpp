#include "fmt/format.h"
#include <QKeyEvent>
#include <QSignalBlocker>
#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "editConversationChoiceForm.hpp"
#include "errorMessage.hpp"

using commoneditor::ui::ErrorMessage;
using mapeditor::controllers::EditConversationChoiceFormController;
using thewarrior::models::ConversationChoice;
using thewarrior::models::ConversationChoiceOption;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;
using thewarrior::models::ConversationNodeTransitionType;

EditConversationChoiceForm::EditConversationChoiceForm(QWidget *parent,
                                                       const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                                       const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds)
    : QDialog(parent),
    ui(Ui::editConversationChoiceFormClass()),
    m_controller(selectedConversationNode, alreadyUsedNodeIds) {
    ui.setupUi(this);
    setWindowIcon(QIcon(":/MapEditor Icon.png"));
    this->setFixedSize(this->geometry().size());
    initializeChoiceTable();
    connectUIActions();
    initializeComboBoxTransitionType(); 
    if (selectedConversationNode.has_value()) {
        auto transition = selectedConversationNode->getTransition();
        ui.lineEditId->setText(selectedConversationNode->getId().c_str());
        const auto &conversationChoice = boost::get<ConversationChoice>(
            selectedConversationNode->getContent());
        ui.lineEditPrompt->setText(conversationChoice.prompt.c_str());
        for (const auto &option : conversationChoice.options) {
            const int row = ui.tableWidgetChoices->rowCount() - 1;
            ui.tableWidgetChoices->item(row, 0)->setText(
                QString::fromStdString(option.text));
            ui.tableWidgetChoices->item(row, 1)->setText(
                QString::fromStdString(option.nextNodeId));
        }

        ui.comboBoxTransitionType->setCurrentIndex(static_cast<int>(transition.getType()));
        if (transition.getType() == ConversationNodeTransitionType::SpecificNode) {
            ui.lineEditNextNodeId->setText(transition.getNextNodeId().c_str());
        }
    }
}

bool EditConversationChoiceForm::isEditMode() const {
    return m_controller.isEditMode();
}

const ConversationNode &EditConversationChoiceForm::getResult() const {
    return m_result;
}

void EditConversationChoiceForm::connectUIActions() {
    connect(ui.pushButtonCancel, &QPushButton::clicked, this, &EditConversationChoiceForm::onPushButtonCancelClick);
    connect(ui.pushButtonOK, &QPushButton::clicked, this, &EditConversationChoiceForm::onPushButtonOKClick);
    connect(ui.tableWidgetChoices, &QTableWidget::itemChanged,
            this, &EditConversationChoiceForm::onChoiceItemChanged);
    connect(ui.comboBoxTransitionType,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &EditConversationChoiceForm::onComboBoxTransitionTypeIndexChanged);
}

void EditConversationChoiceForm::initializeChoiceTable() {
    ui.tableWidgetChoices->setHorizontalHeaderItem(0, new QTableWidgetItem("Choice label"));
    ui.tableWidgetChoices->setHorizontalHeaderItem(1, new QTableWidgetItem("Choice Next Node Id"));
    ui.tableWidgetChoices->setColumnWidth(0, 275);
    ui.tableWidgetChoices->setColumnWidth(1, 180);
    ui.tableWidgetChoices->installEventFilter(this);
    appendEmptyChoiceRow();
}

void EditConversationChoiceForm::appendEmptyChoiceRow() {
    const QSignalBlocker blocker(ui.tableWidgetChoices);
    const int row = ui.tableWidgetChoices->rowCount();
    ui.tableWidgetChoices->insertRow(row);
    ui.tableWidgetChoices->setItem(row, 0, new QTableWidgetItem());
    ui.tableWidgetChoices->setItem(row, 1, new QTableWidgetItem());
}

void EditConversationChoiceForm::initializeComboBoxTransitionType()
{
    ui.comboBoxTransitionType->insertItem(0, "Next In Order");
    ui.comboBoxTransitionType->insertItem(1, "Stop");
    ui.comboBoxTransitionType->insertItem(2, "Specific Node");
}

void EditConversationChoiceForm::onPushButtonCancelClick() {
    reject();
}

void EditConversationChoiceForm::onPushButtonOKClick() {
    std::string id = ui.lineEditId->text().trimmed().toStdString();
    if (id.empty()) {
        ErrorMessage::show("The node id is required.");
        return;
    }
    if (m_controller.isNodeIdAlreadyUsed(id)) {
        ErrorMessage::show(fmt::format("The node id {} already exists in the list.", id));
        return;
    }

    std::string prompt = ui.lineEditPrompt->text().trimmed().toStdString();
    if (prompt.empty()) {
        ErrorMessage::show("The prompt is required.");
        return;
    }

    std::vector<ConversationChoiceOption> options;
    for (int row = 0; row < ui.tableWidgetChoices->rowCount(); ++row) {
        const QString text = ui.tableWidgetChoices->item(row, 0)->text().trimmed();
        const QString optionNextNodeId =
            ui.tableWidgetChoices->item(row, 1)->text().trimmed();
        if (text.isEmpty() && optionNextNodeId.isEmpty()) {
            continue;
        }
        if (text.isEmpty() || optionNextNodeId.isEmpty()) {
            ErrorMessage::show(fmt::format(
                "Choice row {} requires both a label and a next node id.",
                row + 1));
            return;
        }
        options.push_back(ConversationChoiceOption {
            .text = text.toStdString(),
            .nextNodeId = optionNextNodeId.toStdString()
        });
    }
    if (options.size() < 2) {
        ErrorMessage::show("At least two choices are required.");
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
        ConversationChoice {
            .prompt = prompt,
            .options = std::move(options)
        },
        transition
    );
    accept();
}

void EditConversationChoiceForm::onChoiceItemChanged(QTableWidgetItem *item) {
    const int row = item->row();
    const bool isTextEmpty =
        ui.tableWidgetChoices->item(row, 0)->text().trimmed().isEmpty();
    const bool isNextNodeIdEmpty =
        ui.tableWidgetChoices->item(row, 1)->text().trimmed().isEmpty();

    if (isTextEmpty && isNextNodeIdEmpty &&
        ui.tableWidgetChoices->rowCount() > 1) {
        const QSignalBlocker blocker(ui.tableWidgetChoices);
        ui.tableWidgetChoices->removeRow(row);
    } else if (row == ui.tableWidgetChoices->rowCount() - 1 &&
               (!isTextEmpty || !isNextNodeIdEmpty)) {
        appendEmptyChoiceRow();
    }
}

bool EditConversationChoiceForm::eventFilter(QObject *watched, QEvent *event) {
    if (watched != ui.tableWidgetChoices ||
        event->type() != QEvent::KeyPress) {
        return QDialog::eventFilter(watched, event);
    }

    const auto *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() != Qt::Key_Delete) {
        return QDialog::eventFilter(watched, event);
    }

    std::vector<int> selectedRows;
    for (const auto &range : ui.tableWidgetChoices->selectedRanges()) {
        if (range.leftColumn() == 0 &&
            range.rightColumn() == ui.tableWidgetChoices->columnCount() - 1) {
            for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
                selectedRows.push_back(row);
            }
        }
    }

    if (!selectedRows.empty()) {
        std::ranges::sort(selectedRows, std::greater<>());
        const QSignalBlocker blocker(ui.tableWidgetChoices);
        for (const int row : selectedRows) {
            if (ui.tableWidgetChoices->rowCount() == 1) {
                break;
            }
            ui.tableWidgetChoices->removeRow(row);
        }

        bool hasEmptyRow = false;
        for (int row = 0; row < ui.tableWidgetChoices->rowCount(); ++row) {
            hasEmptyRow =
                ui.tableWidgetChoices->item(row, 0)->text().trimmed().isEmpty() &&
                ui.tableWidgetChoices->item(row, 1)->text().trimmed().isEmpty();
            if (hasEmptyRow) {
                break;
            }
        }
        if (!hasEmptyRow) {
            appendEmptyChoiceRow();
        }
    } else {
        const QSignalBlocker blocker(ui.tableWidgetChoices);
        for (QTableWidgetItem *item : ui.tableWidgetChoices->selectedItems()) {
            item->setText({});
        }

        bool emptyRowKept = false;
        for (int row = ui.tableWidgetChoices->rowCount() - 1;
             row >= 0;
             --row) {
            const bool isEmpty =
                ui.tableWidgetChoices->item(row, 0)->text().trimmed().isEmpty() &&
                ui.tableWidgetChoices->item(row, 1)->text().trimmed().isEmpty();
            if (isEmpty && emptyRowKept &&
                ui.tableWidgetChoices->rowCount() > 1) {
                ui.tableWidgetChoices->removeRow(row);
            } else if (isEmpty) {
                emptyRowKept = true;
            }
        }
    }

    return true;
}

void EditConversationChoiceForm::onComboBoxTransitionTypeIndexChanged() {
    ui.labelNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
    ui.lineEditNextNodeId->setVisible(ui.comboBoxTransitionType->currentIndex() == 2);
}
