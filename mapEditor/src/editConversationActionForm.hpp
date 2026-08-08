
#ifndef MAPEDITOR_SRC_EDITCONVERSATIONACTIONFORM_HPP_
#define MAPEDITOR_SRC_EDITCONVERSATIONACTIONFORM_HPP_

#include <qdialog.h>
#include <qlist.h>
#include <string>
#include <vector>
#include "ui_editConversationActionForm.h"
#include "conversationScenario.hpp"
#include "editConversationActionFormController.hpp"
#include "glComponentController.hpp"

class EditConversationActionForm : public QDialog {
Q_OBJECT

 public:
   EditConversationActionForm(QWidget *parent,
                              const mapeditor::controllers::GLComponentController *glComponentController,
                              const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                              const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
   bool isEditMode() const;
   const thewarrior::models::ConversationNode &getResult() const;

 private:
   Ui::editConversationActionFormClass ui;
   mapeditor::controllers::EditConversationActionFormController m_controller;
   thewarrior::models::ConversationNode m_result;
   void connectUIActions();
   void initializeComboBoxTransitionType();
   void initializeMerchantInventoryComboBoxes();
   void onPushButtonCancelClick();
   void onPushButtonOKClick();
   void onComboBoxTransitionTypeIndexChanged();
   void onComboBoxFailureTransitionTypeIndexChanged();
   void onActionTypeChanged();
   void onCheckBoxOptionalMerchantShopChanged();
};

#endif  // MAPEDITOR_SRC_EDITCONVERSATIONACTIONFORM_HPP_
