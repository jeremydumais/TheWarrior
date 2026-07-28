
#ifndef MAPEDITOR_SRC_EDITCONVERSATIONSCENARIOFORM_HPP_
#define MAPEDITOR_SRC_EDITCONVERSATIONSCENARIOFORM_HPP_

#include <qdialog.h>
#include <optional>
#include <string>
#include <vector>
#include "ui_editConversationScenarioForm.h"
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"
#include "qTableWidgetKeyPressWatcher.h"

class EditConversationScenarioForm : public QDialog {
Q_OBJECT

 public:
    EditConversationScenarioForm(QWidget *parent,
                const std::string &resourcesPath,
                const std::optional<thewarrior::models::ConversationScenario> &selectedConversationScenario,
                const std::vector<thewarrior::models::ConversationScenarioId> &alreadyUsedScenarioIds);
    bool isEditMode() const;
    const thewarrior::models::ConversationScenario &getResult() const;

 private:
   Ui::editConversationScenarioFormClass ui;
   mapeditor::controllers::EditConversationScenarioFormController m_controller;
   thewarrior::models::ConversationScenario m_result;
   QTableWidgetKeyPressWatcher tableWidgetNodesKeyWatcher;
   void initializeNodesTable();
   void connectUIActions();
   void refreshNodesTable();
   void onPushButtonCancelClick();
   void onPushButtonOKClick();
   void onPushButtonEditNodeClick();
   void onPushButtonDeleteNodeClick();
   void onTableWidgetNodesDoubleClicked(QTableWidgetItem *item);
   void onTableWidgetNodesKeyPressEvent(int key, int row, int column);
   std::optional<thewarrior::models::ConversationNodeId> getSelectedNodeId() const;

};

#endif  // MAPEDITOR_SRC_EDITCONVERSATIONSCENARIOFORM_HPP_
