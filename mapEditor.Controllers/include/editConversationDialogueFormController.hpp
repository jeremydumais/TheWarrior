#pragma once

#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"

namespace mapeditor::controllers {

class EditConversationDialogueFormController : public EditConversationNodeBaseFormController {
 public:
    EditConversationDialogueFormController(
        const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
        const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
};

}  // namespace mapeditor::controllers
