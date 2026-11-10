#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationActionFormController.hpp"
#include "glComponentController.hpp"

using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;

namespace mapeditor::controllers
{

EditConversationActionFormController::EditConversationActionFormController(
    const GLComponentController *glComponentController,
    const std::optional<ConversationNode> &selectedConversationNode,
    const std::vector<ConversationNodeId> &alreadyUsedNodeIds)
: EditConversationNodeBaseFormController(selectedConversationNode, alreadyUsedNodeIds),
  m_glComponentController(glComponentController) {}

std::vector<std::string> EditConversationActionFormController::getMerchantInventoryNames() const {
    return m_glComponentController->getAlreadyUsedMerchantInventoryNames();
}


} // namespace mapeditor::controllers
