#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"

using boost::algorithm::to_upper_copy;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;

namespace mapeditor::controllers
{

    EditConversationNodeBaseFormController::EditConversationNodeBaseFormController(
        const std::optional<ConversationNode> &selectedConversationNode,
        const std::vector<ConversationNodeId> &alreadyUsedNodeIds)
        : m_selectedConversationNode(selectedConversationNode),
          m_alreadyUsedNodeIds(alreadyUsedNodeIds) {}

    const std::string &EditConversationNodeBaseFormController::getLastError() const
    {
        return m_lastError;
    }

    bool EditConversationNodeBaseFormController::isEditMode() const
    {
        return m_selectedConversationNode.has_value();
    }

    bool EditConversationNodeBaseFormController::isNodeIdAlreadyUsed(const std::string &nodeId) const
    {
        if (m_selectedConversationNode.has_value() &&
            to_upper_copy(m_selectedConversationNode->getId()) == to_upper_copy(nodeId)) {
            return false;
        }

        return std::ranges::find_if(m_alreadyUsedNodeIds,
                                    [&nodeId](const std::string &id){
                                        return to_upper_copy(nodeId) == to_upper_copy(id);
                                    }) != m_alreadyUsedNodeIds.end();
    }

} // namespace mapeditor::controllers
