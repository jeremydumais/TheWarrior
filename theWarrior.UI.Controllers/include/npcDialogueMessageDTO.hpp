#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "messageDTO.hpp"

namespace thewarrior::ui::controllers {

struct NPCDialogueMessageDTO : public MessageDTO {
    std::string npcId;
    std::vector<std::string> dialogueLines;
    size_t currentLineIndex = 0;
    bool hasNextPage = false;
    ~NPCDialogueMessageDTO() override = default;
    MessageDTOType getType() override { return MessageDTOType::NPCDialogueMessage; }
};

}  // namespace thewarrior::ui::controllers
