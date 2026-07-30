#include "fmt/format.h"
#include "conversationController.hpp"

using thewarrior::models::ConversationChoice;
using thewarrior::models::ConversationScenario;
using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationNodeId;
using thewarrior::models::ConversationNodeTransition;

namespace thewarrior::ui::controllers {

bool ConversationController::start(const std::string &npcId, const ConversationScenario &scenario) {
    stop();

    const auto &nodes = scenario.getNodes();
    if (nodes.empty()) {
        return fail("The conversation scenario contains no nodes.");
    }

    for (std::size_t index = 0; index < nodes.size(); ++index) {
        const auto &nodeId = nodes[index].getId();

        if (nodeId.empty()) {
            return fail("A conversation node has an empty id.");
        }

        const auto [_, inserted] = m_nodeIndexes.emplace(nodeId, index);
        if (!inserted) {
            return fail(fmt::format("The conversation contains duplicate node id '{}'.", nodeId));
        }
    }

    m_npcId = npcId;
    m_scenario = scenario;
    m_currentNodeIndex = 0;
    m_state = ConversationState::Running;
    return true;
}

void ConversationController::stop() {
    m_npcId.clear();
    m_scenario.reset();
    m_nodeIndexes.clear();
    m_currentNodeIndex = 0;
    m_state = ConversationState::Inactive;
    m_lastError.clear();
}

ConversationState ConversationController::getState() const {
    return m_state;
}

const std::string &ConversationController::getNPCId() const {
    return m_npcId;
}

const std::string &ConversationController::getLastError() const {
    return m_lastError;
}

const ConversationNode *ConversationController::getCurrentNode() const {
    if (m_state != ConversationState::Running || !m_scenario.has_value()) {
        return nullptr;
    }

    const auto &nodes = m_scenario->getNodes();
    if (m_currentNodeIndex >= nodes.size()) {
        return nullptr;
    }

    return &nodes[m_currentNodeIndex];
}

bool ConversationController::completeCurrentNode() {
    const auto *node = getCurrentNode();
    if (node == nullptr) {
        return fail("There is no active conversation node.");
    }

    return followTransition(node->getTransition());
}

bool ConversationController::followTransition(const ConversationNodeTransition &transition) {

    if (m_state != ConversationState::Running ||
        !m_scenario.has_value()) {
        return fail("There is no active conversation.");
    }

    switch (transition.getType()) {
        case models::ConversationNodeTransitionType::Stop:
            m_state = ConversationState::Completed;
            return true;

        case models::ConversationNodeTransitionType::NextInOrder: {
            const auto nextIndex = m_currentNodeIndex + 1;
            if (nextIndex >= m_scenario->getNodes().size()) {
                m_state = ConversationState::Completed;
            } else {
                m_currentNodeIndex = nextIndex;
            }
            return true;
        }

        case models::ConversationNodeTransitionType::SpecificNode:
            return moveToNode(transition.getNextNodeId());
    }

    return fail("The conversation contains an unknown transition type.");
}

bool ConversationController::selectChoice(std::size_t optionIndex) {

    const auto *node = getCurrentNode();
    if (node == nullptr) {
        return fail("There is no active conversation node.");
    }

    const auto *choice =
        boost::get<ConversationChoice>(&node->getContent());

    if (choice == nullptr) {
        return fail("The active conversation node is not a choice.");
    }

    if (optionIndex >= choice->options.size()) {
        return fail("The selected conversation option is out of range.");
    }

    if (choice->options[optionIndex].nextNodeId == "<stop>") {
        m_state = ConversationState::Completed;
        return true;
    }
    return moveToNode(choice->options[optionIndex].nextNodeId);
}

bool ConversationController::moveToNode(const ConversationNodeId &nodeId) {
    const auto iter = m_nodeIndexes.find(nodeId);
    if (iter == m_nodeIndexes.end()) {
        return fail(fmt::format("Unable to find conversation node '{}'.", nodeId));
    }

    m_currentNodeIndex = iter->second;
    return true;
}

bool ConversationController::fail(const std::string &message) {
    m_lastError = message;
    m_state = ConversationState::Error;
    return false;
}

}  // namespace thewarrior::ui::controllers
