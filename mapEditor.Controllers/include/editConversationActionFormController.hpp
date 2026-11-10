#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"
#include "editConversationNodeBaseFormController.hpp"
#include "glComponentController.hpp"

namespace mapeditor::controllers {

class EditConversationActionFormController : public EditConversationNodeBaseFormController {
 public:
    EditConversationActionFormController(const GLComponentController *glComponentController,
                                         const std::optional<thewarrior::models::ConversationNode> &selectedConversationNode,
                                         const std::vector<thewarrior::models::ConversationNodeId> &alreadyUsedNodeIds);
    std::vector<std::string> getMerchantInventoryNames() const;
 private:
    const GLComponentController *m_glComponentController = nullptr;
};

}  // namespace mapeditor::controllers
