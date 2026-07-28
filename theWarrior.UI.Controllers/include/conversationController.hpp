#pragma once

#include <optional>
#include <string>
#include "conversationScenario.hpp"

namespace thewarrior::ui::controllers {

enum class ConversationState {
    Inactive,
    Running,
    Completed,
    Error
};

class ConversationController {
 public:
    bool start(const std::string &npcId,
               const thewarrior::models::ConversationScenario &scenario);
    void stop();

    ConversationState getState() const;
    const std::string &getNPCId() const;
    const std::string &getLastError() const;

    const thewarrior::models::ConversationNode *getCurrentNode() const;
    bool completeCurrentNode();
    bool followTransition(const thewarrior::models::ConversationNodeTransition &transition);
    bool selectChoice(std::size_t optionIndex);

 private:
    std::string m_npcId;
    std::optional<thewarrior::models::ConversationScenario> m_scenario;
    std::unordered_map<thewarrior::models::ConversationNodeId, std::size_t> m_nodeIndexes;
    std::size_t m_currentNodeIndex = 0;
    ConversationState m_state = ConversationState::Inactive;
    std::string m_lastError;

    bool moveToNode(const thewarrior::models::ConversationNodeId &nodeId);
    bool fail(const std::string &message);
};

}  // namespace thewarrior::ui::controllers
