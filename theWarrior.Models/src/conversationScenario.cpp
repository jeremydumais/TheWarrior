#include "conversationScenario.hpp"
#include <algorithm>
#include <stdexcept>

namespace thewarrior::models {

ConversationNodeTransition ConversationNodeTransition::nextInOrder() {
    ConversationNodeTransition transition;
    transition.m_type = ConversationNodeTransitionType::NextInOrder;
    return transition;
}

ConversationNodeTransition ConversationNodeTransition::stop() {
    ConversationNodeTransition transition;
    transition.m_type = ConversationNodeTransitionType::Stop;
    return transition;
}

ConversationNodeTransition ConversationNodeTransition::toNode(
        const ConversationNodeId &nodeId) {
    if (nodeId.empty()) {
        throw std::invalid_argument(
            "The next conversation node id cannot be empty.");
    }
    ConversationNodeTransition transition;
    transition.m_type = ConversationNodeTransitionType::SpecificNode;
    transition.m_nextNodeId = nodeId;
    return transition;
}

ConversationNodeTransitionType ConversationNodeTransition::getType() const {
    return m_type;
}

const ConversationNodeId &ConversationNodeTransition::getNextNodeId() const {
    return m_nextNodeId;
}

ConversationNode::ConversationNode(const ConversationNodeId &id,
                                   const ConversationNodeContent &content,
                                   const ConversationNodeTransition &transition)
    : m_id(id),
      m_content(content),
      m_transition(transition) {
}

const ConversationNodeId &ConversationNode::getId() const {
    return m_id;
}

const ConversationNodeContent &ConversationNode::getContent() const {
    return m_content;
}

const ConversationNodeTransition &ConversationNode::getTransition() const {
    return m_transition;
}

void ConversationNode::setId(const ConversationNodeId &id) {
    m_id = id;
}

void ConversationNode::setContent(const ConversationNodeContent &content) {
    m_content = content;
}

void ConversationNode::setTransition(const ConversationNodeTransition &transition) {
    m_transition = transition;
}

ConversationScenario::ConversationScenario(
        const ConversationScenarioId &id,
        const StoryId &conditionalStoryIdCompleted,
        const std::vector<ConversationNode> &nodes)
    : m_id(id),
      m_conditionalStoryIdCompleted(conditionalStoryIdCompleted),
      m_nodes(nodes) {
}

ConversationScenario ConversationScenario::fromLegacyDialogueLines(
        const std::vector<std::string> &lines) {
    ConversationScenario scenario;
    scenario.m_id = "default";
    if (!lines.empty()) {
        scenario.m_nodes.emplace_back(
            "dialogue",
            ConversationDialogue { .lines = lines });
    }
    return scenario;
}

const ConversationScenarioId &ConversationScenario::getId() const {
    return m_id;
}

const StoryId &ConversationScenario::getConditionalStoryIdCompleted() const {
    return m_conditionalStoryIdCompleted;
}

const std::vector<ConversationNode> &ConversationScenario::getNodes() const {
    return m_nodes;
}

std::vector<std::string> ConversationScenario::getDialogueLines() const {
    for (const auto &node : m_nodes) {
        const auto *dialogue = boost::get<ConversationDialogue>(&node.getContent());
        if (dialogue != nullptr) {
            return dialogue->lines;
        }
    }
    return {};
}

void ConversationScenario::setId(const ConversationScenarioId &id) {
    m_id = id;
}

void ConversationScenario::setConditionalStoryIdCompleted(const StoryId &storyId) {
    m_conditionalStoryIdCompleted = storyId;
}

void ConversationScenario::setNodes(const std::vector<ConversationNode> &nodes) {
    m_nodes = nodes;
}

void ConversationScenario::setDialogueLines(const std::vector<std::string> &lines) {
    auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [](const ConversationNode &node) {
        return boost::get<ConversationDialogue>(&node.getContent()) != nullptr;
    });
    if (iter != m_nodes.end()) {
        iter->setContent(ConversationDialogue { .lines = lines });
    } else if (!lines.empty()) {
        m_nodes.emplace_back("dialogue", ConversationDialogue { .lines = lines });
    }
}

void ConversationScenario::addNode(const ConversationNode &node) {
    m_nodes.push_back(node);
}

bool ConversationScenario::replaceNode(const ConversationNodeId &id,
                                       const ConversationNode &node) {
    const auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [&id](const ConversationNode &item) {
        return item.getId() == id;
    });
    if (iter == m_nodes.end()) {
        return false;
    }
    *iter = node;
    return true;
}

bool ConversationScenario::removeNode(const ConversationNodeId &id) {
    const auto iter = std::find_if(m_nodes.begin(), m_nodes.end(), [&id](const ConversationNode &item) {
        return item.getId() == id;
    });
    if (iter == m_nodes.end()) {
        return false;
    }
    m_nodes.erase(iter);
    return true;
}

}  // namespace thewarrior::models
