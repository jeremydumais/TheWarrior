
#ifndef MAPEDITOR_SRC_EDITCONVERSATIONDIALOGUEFORM_HPP_
#define MAPEDITOR_SRC_EDITCONVERSATIONDIALOGUEFORM_HPP_

#include <qdialog.h>
#include <qlist.h>
#include <string>
#include <vector>
#include "ui_editConversationDialogueForm.h"
#include "conversationScenario.hpp"
#include "editConversationDialogueFormController.hpp"

class EditConversationDialogueForm : public QDialog {
Q_OBJECT

 public:
   EditConversationDialogueForm(QWidget *parent,
                                const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
   bool isEditMode() const;
   const thewarrior::models::ConversationNode &getResult() const;

 private:
   Ui::editConversationDialogueFormClass ui;
   mapeditor::controllers::EditConversationDialogueFormController m_controller;
   thewarrior::models::ConversationNode m_result;
   void connectUIActions();
   void initializeComboBoxTransitionType();
   void refreshNodesTable();
   void onPushButtonCancelClick();
   void onPushButtonOKClick();
   void onComboBoxTransitionTypeIndexChanged();
   static std::vector<std::string> convertPlainTextToLines(const QString &text);
};

#endif  // MAPEDITOR_SRC_EDITCONVERSATIONDIALOGUEFORM_HPP_
