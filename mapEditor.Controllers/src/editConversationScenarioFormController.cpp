#include "fmt/format.h"
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"

using boost::algorithm::to_upper_copy;
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

bool EditConversationScenarioFormController::isScenarioIdAlreadyUsed(const ConversationScenarioId &scenarioId) const {
    if (m_selectedConversationScenario.has_value() &&
        to_upper_copy(m_selectedConversationScenario->getId()) == to_upper_copy(scenarioId)) {
        return false;
    }

    return std::ranges::find_if(m_alreadyUsedScenarioIds,
                                [&scenarioId](const ConversationScenarioId &id)
                                {
                                    return to_upper_copy(scenarioId) == to_upper_copy(id);
                                }) != m_alreadyUsedScenarioIds.end();
}

const std::vector<ConversationNode> &EditConversationScenarioFormController::getNodes() const {
    return m_conversationNodes;
}

std::vector<ConversationNodeId> EditConversationScenarioFormController::getAlreadyUsedNodeIds() const {
    std::vector<ConversationNodeId> alreadyUsedNodeIds = {};
    std::ranges::transform(m_conversationNodes,
                            back_inserter(alreadyUsedNodeIds),
                            [](ConversationNode const &x)
                            { return x.getId(); });
    return alreadyUsedNodeIds;
}

std::optional<ConversationNode> EditConversationScenarioFormController::getNodeById(const ConversationNodeId &nodeId) const {
    std::optional<ConversationNode> selectedNode = std::nullopt;
    const auto node = std::ranges::find_if(m_conversationNodes,
                                            [&nodeId](const ConversationNode &candidate)
                                            {
                                                return candidate.getId() == nodeId;
                                            });
    if (node != m_conversationNodes.end()) {
        selectedNode = *node;
    }
    return selectedNode;
}

void EditConversationScenarioFormController::addConversationNode(const ConversationNode &node) {
    m_conversationNodes.push_back(node);
}

bool EditConversationScenarioFormController::updateConversationNode(const ConversationNodeId &oldConversationNodeId,
                                                                    const ConversationNode &node) { 
    const auto oldNode = std::ranges::find_if(m_conversationNodes,
                                                [&oldConversationNodeId](const ConversationNode &candidate)
                                                {
                                                    return candidate.getId() == oldConversationNodeId;
                                                });
    if (oldNode == m_conversationNodes.end()) {
        m_lastError = fmt::format("Unable to find the node {0}", oldConversationNodeId);
        return false;
    }

    *oldNode = node;
    return true;
}

bool EditConversationScenarioFormController::removeConversationNode(const ConversationNodeId &oldConversationNodeId) {
    const auto oldNode = std::ranges::find_if(m_conversationNodes,
                                                [&oldConversationNodeId](const ConversationNode &candidate) {
                                                    return candidate.getId() == oldConversationNodeId;
                                                });
    if (oldNode == m_conversationNodes.end()) {
        m_lastError = fmt::format("Unable to find the node {0}", oldConversationNodeId);
        return false;
    }

    m_conversationNodes.erase(oldNode);
    return true;
}

} // namespace mapeditor::controllers
