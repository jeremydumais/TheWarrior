#pragma once

#include <string>
#include "messageDTO.hpp"

namespace thewarrior::ui::controllers {

struct NPCDialogueMessageDTO : public MessageDTO {
    std::string npcId;
    ~NPCDialogueMessageDTO() override = default;
    MessageDTOType getType() override { return MessageDTOType::NPCDialogueMessage; }
};

}  // namespace thewarrior::ui::controllers
