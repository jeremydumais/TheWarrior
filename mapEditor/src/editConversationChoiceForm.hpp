
#ifndef MAPEDITOR_SRC_EDITCONVERSATIONCHOICEFORM_HPP_
#define MAPEDITOR_SRC_EDITCONVERSATIONCHOICEFORM_HPP_

#include <qdialog.h>
#include <qlist.h>
#include <string>
#include <vector>
#include "ui_editConversationChoiceForm.h"
#include "conversationScenario.hpp"
#include "editConversationChoiceFormController.hpp"

class EditConversationChoiceForm : public QDialog
{
   Q_OBJECT

public:
   EditConversationChoiceForm(QWidget *parent,
                              const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                              const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
   bool isEditMode() const;
   const thewarrior::models::ConversationNode &getResult() const;

protected:
   bool eventFilter(QObject *watched, QEvent *event) override;

private:
   Ui::editConversationChoiceFormClass ui;
   mapeditor::controllers::EditConversationChoiceFormController m_controller;
   thewarrior::models::ConversationNode m_result;
   void connectUIActions();
   void initializeChoiceTable();
   void appendEmptyChoiceRow();
   void onPushButtonCancelClick();
   void onPushButtonOKClick();
   void onChoiceItemChanged(QTableWidgetItem *item);
};

#endif // MAPEDITOR_SRC_EDITCONVERSATIONCHOICEFORM_HPP_
