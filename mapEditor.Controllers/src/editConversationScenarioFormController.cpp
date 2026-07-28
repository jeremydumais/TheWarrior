#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"

using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationScenario;
using thewarrior::models::ConversationScenarioId;

namespace mapeditor::controllers
{

EditConversationScenarioFormController::EditConversationScenarioFormController(const std::string &resourcesPath,
                                                                               const std::optional<ConversationScenario> &selectedConversationScenario,
                                                                               const std::vector<ConversationScenarioId> &alreadyUsedScenarioIds)
: m_resourcesPath(resourcesPath),
  m_selectedConversationScenario(selectedConversationScenario),
  m_alreadyUsedScenarioIds(alreadyUsedScenarioIds) {
    if (selectedConversationScenario.has_value()) {
        m_conversationNodes = selectedConversationScenario->getNodes();
    }
}

const std::string &EditConversationScenarioFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditConversationScenarioFormController::getLastError() const {
    return m_lastError;
}

bool EditConversationScenarioFormController::isEditMode() const {
    return m_selectedConversationScenario.has_value();
}

const std::vector<ConversationNode> &EditConversationScenarioFormController::getNodes() const {
    return m_conversationNodes;
}

std::vector<ConversationNodeId> EditConversationScenarioFormController::getAlreadyUsedNodeIds() const {
    std::vector<ConversationNodeId> alreadyUsedNodeIds = {};
    if (m_selectedConversationScenario.has_value()) {
        std::transform(m_selectedConversationScenario->getNodes().begin(),
                        m_selectedConversationScenario->getNodes().end(),
                        back_inserter(alreadyUsedNodeIds),
                        [](ConversationNode const &x)
                        { return x.getId(); });
    }
    return alreadyUsedNodeIds;
}

std::optional<ConversationNode> EditConversationScenarioFormController::getNodeById(const ConversationNodeId &nodeId) const {
    std::optional<ConversationNode> selectedNode = std::nullopt;
    if (m_selectedConversationScenario) {
        const auto &nodes = m_selectedConversationScenario->getNodes();
        const auto node = std::ranges::find_if(nodes,
                                                [&nodeId](const ConversationNode &candidate) {
                                                    return candidate.getId() == nodeId;
                                                });
        if (node != nodes.end()) {
            selectedNode = *node;
        }
    }
    return selectedNode;
}

} // namespace mapeditor::controllers
