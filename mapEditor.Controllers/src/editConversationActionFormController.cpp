#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationActionFormController.hpp"

using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;

namespace mapeditor::controllers
{
    EditConversationActionFormController::EditConversationActionFormController(
        const std::optional<ConversationNode> &selectedConversationNode,
        const std::vector<ConversationNodeId> &alreadyUsedNodeIds)
        : EditConversationNodeBaseFormController(selectedConversationNode,
                                                 alreadyUsedNodeIds) {}

} // namespace mapeditor::controllers
