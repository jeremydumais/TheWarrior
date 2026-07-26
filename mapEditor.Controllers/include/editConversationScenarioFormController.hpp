#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"

namespace mapeditor::controllers {

class EditConversationScenarioFormController {
 public:
    EditConversationScenarioFormController(const std::string &resourcesPath,
                const std::optional<thewarrior::models::ConversationScenario> &selectedConversationScenario,
                const std::vector<thewarrior::models::ConversationScenarioId> &alreadyUsedScenarioIds);
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    bool isEditMode() const;
    std::vector<std::string> getAlreadyUsedNodeIds() const;

 private:
    std::string m_resourcesPath;
    std::string m_lastError;
    const std::optional<thewarrior::models::ConversationScenario> m_selectedConversationScenario;
    const std::vector<thewarrior::models::ConversationScenarioId> m_alreadyUsedScenarioIds;
};

}  // namespace mapeditor::controllers
