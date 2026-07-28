#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"

namespace mapeditor::controllers {

class EditConversationScenarioFormController {
 public:
   EditConversationScenarioFormController(const std::string &resourcesPath,
               const std::optional<thewarrior::models::ConversationScenario> &selectedConversationScenario,
               const std::vector<thewarrior::models::ConversationScenarioId> &alreadyUsedScenarioIds);
   const std::string &getResourcesPath() const;
   const std::string &getLastError() const;
   bool isEditMode() const;
   bool isScenarioIdAlreadyUsed(const thewarrior::models::ConversationScenarioId &scenarioId) const;
   const std::vector<thewarrior::models::ConversationNode> &getNodes() const;
   std::vector<thewarrior::models::ConversationNodeId> getAlreadyUsedNodeIds() const;
   std::optional<thewarrior::models::ConversationNode> getNodeById(const thewarrior::models::ConversationNodeId &nodeId) const;
   void addConversationNode(const thewarrior::models::ConversationNode &node);
   bool updateConversationNode(const thewarrior::models::ConversationNodeId &oldConversationNodeId,
                               const thewarrior::models::ConversationNode &node);
   bool removeConversationNode(const thewarrior::models::ConversationNodeId &oldConversationNodeId);

 private:
   std::string m_resourcesPath;
   std::string m_lastError;
   std::optional<thewarrior::models::ConversationScenario> m_selectedConversationScenario;
   std::vector<thewarrior::models::ConversationScenarioId> m_alreadyUsedScenarioIds;
   std::vector<thewarrior::models::ConversationNode> m_conversationNodes;
};

}  // namespace mapeditor::controllers
