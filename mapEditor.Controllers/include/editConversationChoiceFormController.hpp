#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"

namespace mapeditor::controllers {

class EditConversationChoiceFormController : public EditConversationNodeBaseFormController {
 public:
    EditConversationChoiceFormController(const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                         const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
};

}  // namespace mapeditor::controllers
