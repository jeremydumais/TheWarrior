#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"

namespace mapeditor::controllers {

class EditConversationNodeBaseFormController {
 public:
    EditConversationNodeBaseFormController(const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                           const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
    const std::string &getLastError() const;
    bool isEditMode() const;
    bool isNodeIdAlreadyUsed(const std::string &nodeId) const;

 protected:
    std::string m_lastError;
    const std::optional<thewarrior::models::ConversationNode> m_selectedConversationNode;
    const std::vector<thewarrior::models::ConversationNodeId> m_alreadyUsedNodeIds;
};

}  // namespace mapeditor::controllers
