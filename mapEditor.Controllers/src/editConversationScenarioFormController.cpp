#include <optional>
#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationScenarioFormController.hpp"

using thewarrior::models::ConversationNode;
using thewarrior::models::ConversationScenario;
using thewarrior::models::ConversationScenarioId;

namespace mapeditor::controllers {

EditConversationScenarioFormController::EditConversationScenarioFormController(const std::string &resourcesPath,
                const std::optional<ConversationScenario> &selectedConversationScenario,
                const std::vector<ConversationScenarioId> &alreadyUsedScenarioIds)
: m_resourcesPath(resourcesPath),
m_selectedConversationScenario(selectedConversationScenario),
m_alreadyUsedScenarioIds(alreadyUsedScenarioIds) {}

const std::string &EditConversationScenarioFormController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &EditConversationScenarioFormController::getLastError() const {
    return m_lastError;
}

bool EditConversationScenarioFormController::isEditMode() const {
    return m_selectedConversationScenario.has_value();
}

std::vector<std::string> EditConversationScenarioFormController::getAlreadyUsedNodeIds() const {
    std::vector<std::string> alreadyUsedNodeIds = {};
    if (m_selectedConversationScenario.has_value()) {
        std::transform(m_selectedConversationScenario->getNodes().begin(),
                       m_selectedConversationScenario->getNodes().end(),
                       back_inserter(alreadyUsedNodeIds),
                       [](ConversationNode const& x) { return x.getId(); });
    }
    return alreadyUsedNodeIds;
}

}  // namespace mapeditor::controllers
