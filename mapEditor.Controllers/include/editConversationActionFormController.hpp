#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"

namespace mapeditor::controllers {

class EditConversationActionFormController : public EditConversationNodeBaseFormController {
 public:
    EditConversationActionFormController(const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                         const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
};

}  // namespace mapeditor::controllers
