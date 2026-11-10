#pragma once

#include <string>
#include <vector>
#include "conversationScenario.hpp"

using thewarrior::models::ConversationScenarioId;

namespace mapeditor::controllers {

struct ConversationScenarioSummaryDTO {
    ConversationScenarioId id;
    size_t nodeCount;
};


}  // namespace mapeditor::controllers


